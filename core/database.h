#ifndef DATABASE_DATABASE_H
#define DATABASE_DATABASE_H

#include <functional>
#include <memory>
#include <string>
#include <map>

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

class DatabaseQuery {
public:
	virtual ~DatabaseQuery();

	std::string query(int32_t cat, int32_t func); // Returns corresponding SQL string, this is sufficient for most cases
	std::string query_volume(int vol, int func); // Return the SQL string for the specified volume.
protected:
	virtual load_sql_strings() = 0;
private:
	std::map<int64_t, std::string> sqls_;
};

class DatabaseRegistry {
public:
	static void register_database(std::function<DbConnection()> fab) { dbc_fab_ = fab; }
	static void install_query(DatabaseQuery*);
	static void close_database();
	static DbConnection get_shared_dbc();
	static std::function<DbConnection()> get_dbc_fab();
	static DatabaseQuery* get_query();
private:
	static DbConnection dbc_;
	static std::function<DbConnection()> dbc_fab_;
	static std::shared_ptr<DatabaseQuery> query_;
};

#endif
