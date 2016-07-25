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

void DatabaseRegistry::install_sql_provider(std::unique_ptr<SQLProvider> p)
{
	provider_ = std::move(p);
}

SQLProvider* DatabaseRegistry::get_sql_provider()
{
	return provider_.get();
}

DbConnection DatabaseRegistry::dbc_;
std::function<DbConnection()> DatabaseRegistry::dbc_fab_;
std::unique_ptr<SQLProvider> DatabaseRegistry::provider_;
