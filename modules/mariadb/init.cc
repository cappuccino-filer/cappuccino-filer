/*
 * Demo 0: Basic module
 *
 * The module loader will check every shared library under specified path(s),
 * and then check the existence of entrance functions: cappuccino_filer_module_init and
 * cappuccino_filer_module_term
 *
 * After that the module loader will call cappuccino_filer_module_init. A successful
 * loaded module should return a non-negative integer.
 *
 * On program exit, the loader will call cappuccino_filer_module_term.
 *
 */
#include <pref.h>
#include <database.h>
#include <soci/soci.h>
#include <boost/format.hpp>
#include <string>
#include <memory>
#include <QDebug>

using std::string;

extern "C" {

int cappuccino_filer_module_init()
{
	auto reg = Pref::instance()->get_registry();
	try {
		auto user = reg.get<string>("mariadb.user");
		auto password = reg.get<string>("mariadb.password");
		auto database = reg.get<string>("mariadb.database");
		auto lambda = [user,password,database] () -> DbConnection {
			boost::format fmt("db=%1% user=%2% pass='%3%'");
			string conf = boost::str(fmt % database % user % password);
			return std::make_shared<soci::session>("mysql",
					conf);
		};
		DatabaseRegistry::register_database(lambda);

		auto dbc = DatabaseRegistry::get_shared_dbc();
		if (reg.get<bool>("mariadb.debug", false)) {
			*dbc << "DROP TABLE IF EXISTS tab_volumes";
		}
	} catch (ptree::bad_path& e) {
		qDebug() << " Unable to access perference " << e.what();
		return -1;
	} catch (soci::soci_error& e) {
		qDebug() << "Error during database conection: " << e.what();
		return -1;
	}

	return 0;
}

int cappuccino_filer_module_term()
{
	DatabaseRegistry::close_database();
}

};
