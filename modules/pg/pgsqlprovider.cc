#include "pgsqlprovider.h"
#include <database_query_table.h>

using namespace query;

PGProvider::PGProvider()
{
	sqls_ = {
#include "meta_sql_table.h"
		,
#include "volume_sql_table.h"
		,
#include "tag_sql_table.h"
	};
}
