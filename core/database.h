#ifndef DATABASE_DATABASE_H
#define DATABASE_DATABASE_H

#include <memory>
#include <sqlpp11/mysql/mysql.h>

/*
 * TODO: database interface
 *
 * We'll keep different kinds of information in our DB, so the interface
 * should be as general as possible.
 *
 * Maybe this class should be moved to core
 */

class Pref;

using DatabasePtr = std::shared_ptr<sqlpp::mysql::connection>;

class DatabaseRegistry {
public:
	static void register_database(sqlpp::mysql::connection* db) { db_.reset(db); }
	static void close_database();
	static DatabasePtr get_db();
private:
	static DatabasePtr db_;
};

#endif
