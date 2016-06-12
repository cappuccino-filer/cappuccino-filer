/*
 * Demo 4: Basic actor
 *
 * The implementation of an API should be an actor, and installed by calling
 * Pref::install_actor(). This actor should accept message of
 * shared_ptree
 */

#include <pref.h>
#include <json.h>


namespace {

caf::behavior mkecho(caf::event_based_actor* self)
{
	return { [](shared_ptree pt)
		{
			return pt;
		}
	};
}

const char* apipath = "/api/demo04";

}

extern "C" {
int cappuccino_filer_module_init()
{
	caf::actor echo = caf::spawn(mkecho);
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
