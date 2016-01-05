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

namespace {

caf::behavior mkrelay(caf::event_based_actor* self, DatabasePtr db)
{
	shared_ptree formaterror = json_mkerror("Invalid request format");
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
		}
	};
}

const char* apipath = "/api/fstat";
// TODO: readdir

}

extern "C" {

int draft_module_init()
{
	auto db = std::shared_ptr<Database>(nullptr);
	caf::actor dbactor = caf::spawn(mkrelay, db);
	Pref::instance()->install_actor(apipath, dbactor);
	return 0;
}

int draft_module_term()
{
	caf::anon_send_exit(Pref::instance()->uninstall_actor(apipath),
			caf::exit_reason::user_shutdown);
	return 0;
}

};
