/*
 * Storage module entry point
 *
 * TODO: currently Database object is null, but it should be a cache, so
 * that if a device got offline or content got destroyed, the system still be
 * able to tell what happened.
 *
 */

#include "decl.h"
#include <pref.h>

#include <json.h>
#include "filestat.h"
#include "readdir.h"
#include "volume.h"
#include <launcher.h>

namespace {

shared_ptree formaterror;

caf::behavior mkfstatrelay(caf::event_based_actor* self, DbConnection db)
{
	return { [=](shared_ptree pt)
		{
			try {
				auto path = pt->get<string>("path");
				auto finfo = FileStat::create(db, path);
				return finfo.mkptree();
			} catch (const boost::property_tree::ptree_bad_path&) {
				return formaterror;
			} catch (const string& errmsg) {
				return json_mkerror(errmsg.c_str());
			}
			return pt;
		},
	};
}

caf::behavior mklsrelay(caf::event_based_actor* self, DbConnection db)
{
	return { [=](shared_ptree pt)
		{
			try {
				auto path = pt->get<string>("path");
				auto dent = ReadDir::create(db, path);
				dent->refresh();
				return dent->mkptree();
			} catch (const boost::property_tree::ptree_bad_path&) {
				return formaterror;
			} catch (const string& errmsg) {
				return json_mkerror(errmsg.c_str());
			}
			return pt;
		},
	};
}

caf::behavior mkupdatedb(caf::event_based_actor* self, DbConnection db)
{
	return { [=](shared_ptree pt)
		{
			// Sync to latest status
			Volume::instance()->scan(db);
			return Launcher::instance()->launch("updatedb", pt);
		},
	};
}

const char* apipath = "/api/fstat";
const char* lsapi = "/api/ls";
const char* updatedbapi = "/api/updatedb";
// TODO: readdir

}

extern "C" {

int cappuccino_filer_module_init()
{
	formaterror = json_mkerror("Invalid request format");

	auto db = DatabaseRegistry::get_shared_dbc();
	caf::actor dbactor = caf::spawn(mkfstatrelay, db);
	Pref::instance()->install_actor(apipath, dbactor);
	Pref::instance()->install_actor(lsapi, caf::spawn(mklsrelay, db));
	Pref::instance()->install_actor(updatedbapi, caf::spawn(mkupdatedb, db));

	Volume::instance()->scan(db);

	return 0;
}

int cappuccino_filer_module_term()
{
	caf::anon_send_exit(Pref::instance()->uninstall_actor(apipath),
			caf::exit_reason::user_shutdown);
	caf::anon_send_exit(Pref::instance()->uninstall_actor(lsapi),
			caf::exit_reason::user_shutdown);
	caf::anon_send_exit(Pref::instance()->uninstall_actor(updatedbapi),
			caf::exit_reason::user_shutdown);
	return 0;
}

};

int storage_module_init()
{
	cappuccino_filer_module_init();
}

int storage_module_term()
{
	cappuccino_filer_module_term();
}

