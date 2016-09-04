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
#include <database_query_table.h>

using std::string;

extern "C" {

int cappuccino_filer_module_init()
{
	auto reg = Pref::instance()->get_registry();
	try {
		auto database = reg.get<string>("pg.database");
		auto lambda = [database] () -> DbConnection {
			string conf = "dbname='" + database + "'"; 
			auto ret = std::make_shared<soci::session>("postgresql",
					conf);
			(*ret) << "SET CLIENT_ENCODING TO 'SQL_ASCII';";
			ret->commit();
			return ret;
		};
		DatabaseRegistry::register_database(lambda);
		DatabaseRegistry::install_sql_provider(std::make_unique<PGProvider>());

		auto dbc = DatabaseRegistry::get_shared_dbc();
		bool refresh_database = reg.get("debug.refresh_database", false);
		qDebug() << "debug.refresh_database = " << refresh_database;
		if (refresh_database) {
			soci::transaction tr1(*dbc);
			soci::rowset<soci::row> rows = ((dbc->prepare) << RETRIVE_SQL_QUERY(query::meta, DROP_ALL_TABLES));
			std::vector<string> sqls;
			for(auto& row : rows) {
				sqls.emplace_back(row.get<string>(0));
			}
			for(const auto& sql: sqls) {
				(*dbc) << sql;
				qDebug() << "Executing: " << sql.c_str();
			}
			tr1.commit();
			qDebug() << "DROP ALL TABLES FOR A REFRESH START";
		}
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
