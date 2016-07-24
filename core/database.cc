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

void DatabaseRegistry::install_query(DatabaseQuery* query)
{
	query_.reset(query);
}

DatabaseQuery* DatabaseRegistry::get_query()
{
	return query_.get();
}

DbConnection DatabaseRegistry::dbc_;
std::function<DbConnection()> DatabaseRegistry::dbc_fab_;
std::shared_ptr<DatabaseQuery> DatabaseRegistry::query_;
