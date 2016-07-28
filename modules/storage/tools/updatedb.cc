/*
 * Note: it is meanless to use epoll on readdir or fstat, which is not
 * supported by at least ext4.
 */

#include <unistd.h>
#include <pref.h>
#include <iostream>
#include <sstream>
#include <string>
#include <json.h>
#include <database.h>
#include <database_query_table.h>
#include "../init.h"
#include <QDebug>
#include "../syncer.h"
#include <soci/soci.h>

namespace {

	shared_ptree init()
	{
		ptree pt_pref;
		uint32_t prefsize, reqsize;
		std::cin.read((char*)&prefsize, sizeof(uint32_t));
		std::cin.read((char*)&reqsize, sizeof(uint32_t));
		std::stringstream prefss, reqss;
		std::copy_n(std::istream_iterator<char>(std::cin),
				prefsize,
				std::ostream_iterator<char>(prefss));
		std::copy_n(std::istream_iterator<char>(std::cin),
				reqsize,
				std::ostream_iterator<char>(reqss));
		prefss.seekg(0, std::ios::beg);
		reqss.seekg(0, std::ios::beg);
		qDebug() << prefss.str().c_str();
		qDebug() << reqss.str().c_str();

		pt_pref.load_from(prefss);
		Pref::instance()->set_registry(pt_pref);
		Pref::instance()->scan_modules();

		ptree pt_req;
		pt_req.load_from(reqss);
		return pt_req;
	}

	shared_ptree read_req()
	{
		ptree pt_req;
		pt_req.load_from(std::cin);
		return pt_req;
	}

	DbConnection db;
	std::unique_ptr<Syncer> syncer;
	SQLProvider* sql_provider;

	bool connect_sql()
	{
		try {
			auto dbname = Pref::instance()->get_pref("core.database");
			Pref::instance()->scan_modules();
			Pref::instance()->load_specific_module("lib" + dbname); // Now database is avaliable at DatabaseRegistry::get_db()
		} catch (std::exception& e) {
			qDebug() << e.what();
		}
		db = DatabaseRegistry::get_shared_dbc();
		sql_provider = DatabaseRegistry::get_sql_provider();
		return !!db;
	}

	void scan_cwd()
	{
		std::vector<std::string> subdirs;
		syncer->sync_cwd(subdirs);
		for (const auto& subdirname : subdirs) {
			if (!::chdir(subdirname.c_str())) {
				scan_cwd();
				::chdir("..");
			}
		}
	}

	void scan_main(const std::string& path)
	{
		chdir(path.c_str());
		scan_cwd();
	}

	void scan_volume(const std::string& base)
	{
		int volid = -1;
		bool need_create_volume_table = false;
		uint64_t root_ino;
		string root_path;
		struct stat objstat;
		::lstat(base.c_str(), &objstat);

		/*
		 * Locate the tracking ID, which identifies the table we need to
		 * update
		 */
		soci::transaction tr1(*db);
		soci::rowset<soci::row> mpoints = (db->prepare << "SELECT mount, trID from volumes_table,tracking_table WHERE volumes_table.uuid = tracking_table.uuid;");
		for(auto& row : mpoints) {
			auto mp = row.get<string>(0);
			auto id = row.get<int>(1);
			struct stat matchingstat;
			::lstat(mp.c_str(), &matchingstat);
			if (matchingstat.st_dev == objstat.st_dev) {
				volid = id;
				root_path = mp;
			}
		}
		/*
		 * Insert row to tracking table, which generates a new tracking ID.
		 */
		if (volid < 0) {
			soci::rowset<soci::row>  mpoints = (db->prepare << "SELECT mount,uuid from volumes_table;");
			for(auto& row : mpoints) {
				auto mp = row.get<string>(0);
				auto uuid = row.get<string>(1);
				struct stat matchingstat;
				::lstat(mp.c_str(), &matchingstat);
				if (matchingstat.st_dev == objstat.st_dev) {
					*db << RETRIVE_SQL_QUERY(query::meta,
							ADD_NEW_TRACKING_WITH_RETURN),
						soci::use(uuid),
						soci::into(volid);
					root_path = mp;
					need_create_volume_table = true;
					root_ino = uint64_t(matchingstat.st_ino);
				}
			}
		}
		qDebug() << "Scanning Vol " << volid << " with root path " << root_path.c_str();

		/*
		 * Create table if not exists.
		 */
		if (need_create_volume_table) {
			(*db) << sql_provider->query_volume(volid, query::volume::CREATE);
			(*db) << sql_provider->query_volume(volid, query::volume::UPDATE_ROOT_INODE), soci::use(root_ino);
		}
		tr1.commit();

		syncer = std::make_unique<Syncer>(db, volid);
		db->begin();
		(*db) << sql_provider->query_volume(volid, query::volume::SYNC_INIT_ACK);
		scan_main(root_path);
		(*db) << sql_provider->query_volume(volid, query::volume::SYNC_CLEAR_NON_ACK);
		db->commit();
	}
};

int main(int argc, char* argv[])
{
	std::vector<std::string> base_array;
	if (!isatty(fileno(stdin))) {
		// Normally updatedb should be launched by cappuccino-filer
		// with property tree sent from stdin.
		auto pt_req = init();
		// TODO: disable debugging by changing "/boot" to "" 
		try {
			for (const auto& item : pt_req.get_child("paths")) {
				base_array.emplace_back(
						item.get<std::string>()
					);
			}
		} catch (std::exception& e) {
			qDebug() << e.what();
		}
		if (base_array.empty())
			return 0;
		qDebug() << "Going to scan ";
		for(const auto& path : base_array) {
			qDebug() << "\t\t" << path.c_str();
		}
	} else if (argc > 1) {
		// If started from console, then assuming it's debugging.
		for(int i = 1; i < argc; i++)
			base_array.emplace_back(argv[i]);
	} else {
		base_array.emplace_back("/boot");
	}

	if (!connect_sql()) {
		std::cerr << "Unable to establish connection to database, exiting\n";
		return -1;
	}

	for(const auto& base : base_array) {
		scan_volume(base);
	}

	return 0;
}
