#include "database.h"
#include <stdexcept>

void DatabaseRegistry::register_database(std::function<DbConnection()> fab)
{
	dbc_fab_ = fab;
}

void DatabaseRegistry::close_database()
{
	dbc_.reset();
	dbc_fab_ = nullptr;
	provider_.reset();
}

DbConnection DatabaseRegistry::get_shared_dbc()
{
	if (!dbc_) {
		try {
			dbc_ = dbc_fab_();
		} catch (std::bad_function_call& ) {
			throw std::runtime_error("Failed to connect any database. Check the configuration, or no database server are running?");
		}
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
