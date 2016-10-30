#include <pref.h>
#include <json.h>
#include <database.h>
#include <database_query_table.h>
#include "tag.h"
#include "filetag.h"
#include <QDebug>
#include <soci/soci.h>

namespace {

caf::behavior mktag(caf::event_based_actor* self)
{
	return { [self](shared_ptree pt)
		{
			auto tagaction = TagActionFab::fab(pt);
			try {
				return tagaction->act();
			} catch (std::exception& e) {
				qDebug() << e.what();
			}
			return ptree();
		}
	};
}

caf::behavior mkftag(caf::event_based_actor* self)
{
	return { [self](shared_ptree pt)
		{
			auto ftagaction = FileTagActionFab::fab(pt);
			return ftagaction->act();
		}
	};
}

const char* tagapipath = "/api/tag";
const char* ftagapipath = "/api/filetag";

}

extern "C" {

int cappuccino_filer_module_init()
{
	// Initialize tag related tables
	auto dbc = DatabaseRegistry::get_shared_dbc();
	soci::transaction tr(*dbc);
	(*dbc) << RETRIVE_SQL_QUERY(query::tag, TABLE_CREATION);
	(*dbc) << RETRIVE_SQL_QUERY(query::tag, FUNCTION_CREATION);
	tr.commit();

	caf::actor tag = caf::spawn(mktag);
	caf::actor ftag = caf::spawn(mkftag);
	Pref::instance()->install_actor(tagapipath, tag);
	Pref::instance()->install_actor(ftagapipath, ftag);
	return 0;
}

int cappuccino_filer_module_term()
{
	caf::anon_send_exit(Pref::instance()->uninstall_actor(tagapipath),
			caf::exit_reason::user_shutdown);
	caf::anon_send_exit(Pref::instance()->uninstall_actor(ftagapipath),
			caf::exit_reason::user_shutdown);
	return 0;
}

};
