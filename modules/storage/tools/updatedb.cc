/*
 * Note: it is meanless to use epoll on readdir or fstat, which is not
 * supported by at least ext4.
 */

#include <unistd.h>
#include <pref.h>
#include <iostream>
#include <string>
#include <json.h>
#include <database.h>
#include <dirent.h>
#include "../init.h"
#include "../dbutil.h"
#include <QDebug>
#include "../syncer.h"
#include <soci/soci.h>

namespace {

	void init_pref()
	{
		auto pt_pref = std::make_shared<ptree>();
		json_read_from_stream(std::cin, *pt_pref);
		Pref::instance()->set_registry(pt_pref);
		Pref::instance()->scan_modules();
	}

	shared_ptree read_req()
	{
		auto pt_req = std::make_shared<ptree>();
		json_read_from_stream(std::cin, *pt_req);
		return pt_req;
	}

	DbConnection db;
	std::unique_ptr<Syncer> syncer;

	bool connect_sql()
	{
		auto dbname = Pref::instance()->get_pref("core.database");
		Pref::instance()->scan_modules();
		Pref::instance()->load_specific_module("lib" + dbname); // Now database is avaliable at DatabaseRegistry::get_db()
		db = DatabaseRegistry::get_shared_dbc();
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
};

int main(int argc, char* argv[])
{
	std::string base;
	int volid = -1;
	bool need_create_volume_table = false;
	if (!isatty(fileno(stdin))) {
		// Normally updatedb should be launched by cappuccino-filer
		// with property tree sent from stdin.
		init_pref();
		auto pt_req = read_req();
		// TODO: disable debugging by changing "/boot" to "" 
		base = pt_req->get("path", "");
		if (base.empty())
			return 0;
	} else if (argc > 1) {
		// If started from console, then assuming it's debugging.
		base = argv[1];
	} else {
		base = "/boot";
	}

	if (!connect_sql()) {
		std::cerr << "Unable to establish connection to database, exiting\n";
		return -1;
	}

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
		if (matchingstat.st_dev == objstat.st_dev)
			volid = id;
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
				*db << "INSERT INTO tracking_table(uuid) VALUES(:1)", soci::use(uuid);
				*db << "SELECT LAST_INSERT_ID();", soci::into(volid);
				need_create_volume_table = true;
			}
		}
	}
	qDebug() << "Scanning Vol " << volid;

	/*
 	 * Create table if not exists.
	 */
	if (need_create_volume_table)
		create_volume_table(db, volid);
	tr1.commit();

	syncer = std::make_unique<Syncer>(db, volid);
	db->begin();
	set_voluem_table_before_sync(db, volid);
	scan_main(base);
	clean_voluem_table_after_sync(db, volid);
	db->commit();

	return 0;
}
