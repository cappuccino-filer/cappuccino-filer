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
#include "../readdir.h"
#include "../syncer.h"
#include <soci/soci.h>

namespace {

	void init_pref()
	{
		auto pt_pref = std::make_shared<ptree>();
		boost::property_tree::read_json(std::cin, *pt_pref);
		Pref::instance()->set_registry(pt_pref);
		Pref::instance()->scan_modules();
	}

	shared_ptree read_req()
	{
		auto pt_req = std::make_shared<ptree>();
		boost::property_tree::read_json(std::cin, *pt_req);
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
	// If started from console, then assuming it's debugging.
	if (!isatty(fileno(stdin))) {
		init_pref();
		auto pt_req = read_req();
		// TODO: disable debugging by changing "/boot" to "" 
		base = pt_req->get("path", "");
		if (base.empty())
			return 0;
	} else if (argc > 1) {
		base = argv[1];
	} else {
		base = "/boot";
	}

	if (!connect_sql()) {
		std::cerr << "Unable to establish connection to database, exiting\n";
		return -1;
	}

	db->begin();
	*db << R"xxx(CREATE TABLE IF NOT EXISTS vol_0_inode_table (inode BIGINT PRIMARY KEY, size BIGINT NOT NULL, hash BLOB(32) NULL, mtime_sec BIGINT NULL, mtime_nsec BIGINT NULL, last_check DATETIME NULL, ack BOOLEAN);)xxx";
	*db << R"xxx(CREATE TABLE IF NOT EXISTS vol_0_dentry_table (dnode BIGINT NOT NULL, name VARCHAR(255) NOT NULL, inode BIGINT NOT NULL, ack BOOLEAN, PRIMARY KEY (dnode, name) );)xxx";
	db->commit();

	syncer = std::make_unique<Syncer>(db, 0);
	db->begin();
	*db << R"(UPDATE vol_0_dentry_table SET ack = false;)";
	*db << R"(UPDATE vol_0_inode_table SET ack = false;)";
	scan_main(base);
	*db << R"(DELETE FROM vol_0_dentry_table WHERE ack = false;)";
	*db << R"(DELETE FROM vol_0_inode_table WHERE ack = false;)";
	db->commit();

	return 0;
}
