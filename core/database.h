#ifndef DATABASE_DATABASE_H
#define DATABASE_DATABASE_H

#include <functional>
#include <memory>
#include <string>
#include <map>
#include "sqlprovider.h" // Thus modules needn't include sqlprovider.h separately.

/*
 * TODO: database interface
 *
 * We'll keep different kinds of information in our DB, so the interface
 * should be as general as possible.
 *
 * Maybe this class should be moved to core
 */

namespace soci {
	class session;
};

class Pref;

using DbConnection = std::shared_ptr<soci::session>;

class DatabaseRegistry {
public:
	static void register_database(std::function<DbConnection()> fab);
	static void install_sql_provider(std::unique_ptr<SQLProvider>);
	static void close_database();
	static DbConnection get_shared_dbc();
	static std::function<DbConnection()> get_dbc_fab();
	static SQLProvider* get_sql_provider();
private:
	static DbConnection dbc_;
	static std::function<DbConnection()> dbc_fab_;
	static std::unique_ptr<SQLProvider> provider_;
};

#define RETRIVE_SQL_QUERY(name_space, action) 		\
	(DatabaseRegistry::get_sql_provider()->query(	\
		name_space::cat_id,			\
		name_space::action))

#endif
