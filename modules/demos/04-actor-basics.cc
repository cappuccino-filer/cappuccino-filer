/*
 * Demo 4: Basic actor
 *
 * The implementation of an API should be an actor, and installed by calling
 * Pref::install_actor(). This actor should accept message of
 * std::shared_ptr<boost::property_tree::ptree>.
 */

#include <pref.h>
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>


namespace {

caf::behavior mkecho(caf::event_based_actor* self)
{
	return { [](std::shared_ptr<boost::property_tree::ptree> pt)
		{
			return pt;
		}
	};
}

const char* apipath = "/api/demo04";

}

extern "C" {
int draft_module_init()
{
	caf::actor echo = caf::spawn(mkecho);
	Pref::instance()->install_actor(apipath, echo);
	return 0;
}

int draft_module_term()
{
	caf::anon_send_exit(Pref::instance()->uninstall_actor(apipath),
			caf::exit_reason::user_shutdown);
	return 0;
}

};
