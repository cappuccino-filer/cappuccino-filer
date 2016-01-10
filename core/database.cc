#include "database.h"

void DatabaseRegistry::close_database()
{
	db_.reset();
}

DatabasePtr DatabaseRegistry::get_db()
{
	return db_;
}

DatabasePtr DatabaseRegistry::db_;
