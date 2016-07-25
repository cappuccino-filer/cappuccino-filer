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
#include <string>
#include <memory>
#include <QDebug>
#include "pgsqlprovider.h"

using std::string;

extern "C" {

int cappuccino_filer_module_init()
{
	auto reg = Pref::instance()->get_registry();
	try {
		auto database = reg.get<string>("pg.database");
		auto lambda = [database] () -> DbConnection {
			string conf = "dbname='" + database + "'"; 
			return std::make_shared<soci::session>("postgresql",
					conf);
		};
		DatabaseRegistry::register_database(lambda);
		DatabaseRegistry::install_sql_provider(std::make_unique<PGProvider>());

		auto dbc = DatabaseRegistry::get_shared_dbc();
	} catch (ptree::bad_path& e) {
		qDebug() << " Unable to access perference " << e.what();
		return -1;
	} catch (soci::soci_error& e) {
		qDebug() << "Error during database conection: " << e.what();
		DatabaseRegistry::close_database();
		return -1;
	}

	return 0;
}

int cappuccino_filer_module_term()
{
	DatabaseRegistry::close_database();
}

};
