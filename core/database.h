#ifndef DATABASE_DATABASE_H
#define DATABASE_DATABASE_H

#include <functional>
#include <memory>

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
	static void register_database(std::function<DbConnection()> fab) { dbc_fab_ = fab; }
	static void close_database();
	static DbConnection get_shared_dbc();
	static std::function<DbConnection()> get_dbc_fab();
private:
	static DbConnection dbc_;
	static std::function<DbConnection()> dbc_fab_;
};

#endif
