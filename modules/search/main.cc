/*
 * Demo 4: Basic actor
 *
 * The implementation of an API should be an actor, and installed by calling
 * Pref::install_actor(). This actor should accept message of
 * shared_ptree
 */

#include <pref.h>
#include <json.h>
#include "searcher.h"

namespace {

caf::behavior mksearch(caf::event_based_actor* self)
{
	return { [](shared_ptree pt)
		{
			auto searcher = SearcherFab::fab(pt);
			return searcher->do_search();
		}
	};
}

const char* apipath = "/api/locate";

}

extern "C" {

int cappuccino_filer_module_init()
{
	caf::actor echo = caf::spawn(mksearch);
	Pref::instance()->install_actor(apipath, echo);
	return 0;
}

int cappuccino_filer_module_term()
{
	caf::anon_send_exit(Pref::instance()->uninstall_actor(apipath),
			caf::exit_reason::user_shutdown);
	return 0;
}

};
