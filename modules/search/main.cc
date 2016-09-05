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
#include "search_cache.h"
#include <QDebug>

namespace {

using cache_expunge = caf::atom_constant<caf::atom("expunge")>;

ssize_t get_timeout()
{
	auto reg = Pref::instance()->get_registry();
	return reg.get("searcher.cache_time", 0);
}

caf::behavior mksearch(caf::event_based_actor* self)
{
	return { [self](shared_ptree pt)
		{
			auto timeout = get_timeout();
			if (timeout > 0)
				self->delayed_send(self, std::chrono::seconds(timeout + 1), cache_expunge::value);
			auto searcher = SearcherFab::fab(pt);
			return searcher->do_search();
		},
		[self](cache_expunge) 
		{
			auto timeout = get_timeout();
			qDebug() << "Expunge old cache lines";
			SearcherFab::get_cache().expunge_outdated_lines();
			if (SearcherFab::get_cache().size() > 0 && timeout > 0)
				self->delayed_send(self, std::chrono::seconds(timeout + 1), cache_expunge::value);
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
