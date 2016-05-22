#include "database.h"

void DatabaseRegistry::close_database()
{
	dbc_.reset();
}

DbConnection DatabaseRegistry::get_shared_dbc()
{
	if (!dbc_) {
		dbc_ = dbc_fab_();
	}
	return dbc_;
}

DbConnection DatabaseRegistry::dbc_;
std::function<DbConnection()> DatabaseRegistry::dbc_fab_;
