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

namespace {

shared_ptree formaterror;

caf::behavior mkfstatrelay(caf::event_based_actor* self, DatabasePtr db)
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

caf::behavior mklsrelay(caf::event_based_actor* self, DatabasePtr db)
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

const char* apipath = "/api/fstat";
const char* lsapi = "/api/ls";
// TODO: readdir

}

extern "C" {

int draft_module_init()
{
	formaterror = json_mkerror("Invalid request format");

	auto db = DatabaseRegistry::get_db();
	caf::actor dbactor = caf::spawn(mkfstatrelay, db);
	Pref::instance()->install_actor(apipath, dbactor);
	Pref::instance()->install_actor(lsapi, caf::spawn(mklsrelay, db));
	Volume::instance()->scan(db);
	return 0;
}

int draft_module_term()
{
	caf::anon_send_exit(Pref::instance()->uninstall_actor(apipath),
			caf::exit_reason::user_shutdown);
	caf::anon_send_exit(Pref::instance()->uninstall_actor(lsapi),
			caf::exit_reason::user_shutdown);
	return 0;
}

};
