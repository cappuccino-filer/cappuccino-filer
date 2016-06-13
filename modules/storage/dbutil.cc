#include "dbutil.h"
#include <string>
#include <boost/algorithm/string.hpp>
#include <soci/soci.h>

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

namespace {
	const char* create_volum_sql[] = {
		"CREATE TABLE IF NOT EXISTS vol_#id_inode_table (inode BIGINT PRIMARY KEY, size BIGINT NOT NULL, hash BLOB(32) NULL, mtime_sec BIGINT NULL, mtime_nsec BIGINT NULL, last_check DATETIME NULL, ack BOOLEAN);",
		"CREATE UNIQUE INDEX idx_inode ON vol_#id_inode_table (inode);",
		"CREATE TABLE IF NOT EXISTS vol_#id_dentry_table (dnode BIGINT NOT NULL, name VARCHAR(255) NOT NULL, inode BIGINT NOT NULL, ack BOOLEAN, PRIMARY KEY (dnode, name) );",
		"CREATE UNIQUE INDEX idx_dentry ON vol_#id_dentry_table (dnode, name);",
	};
	const char* volume_set_ack_false[] = {
		"UPDATE vol_#id_dentry_table SET ack = false;",
		"UPDATE vol_#id_inode_table SET ack = false;",
	};

	void exec_sql_for_volume(DbConnection dbc,
			int volid,
			const char* sqltempl[],
			int array_size)
	{
		for(int i = 0; i < array_size; i++) {
			std::string sql(sqltempl[i]);
			boost::replace_all(sql, "#id", std::to_string(volid));
			*dbc << sql;
		}
	}

	const char* volume_clear_non_ack[] = {
		"DELETE FROM vol_#id_dentry_table WHERE ack = false;",
		"DELETE FROM vol_#id_inode_table WHERE ack = false;"
	};
};

void create_volume_table(DbConnection dbc, int volid)
{
	exec_sql_for_volume(dbc, volid, create_volum_sql, ARRAY_SIZE(create_volum_sql));
}

void set_voluem_table_before_sync(DbConnection dbc, int volid)
{
	exec_sql_for_volume(dbc, volid, volume_set_ack_false, ARRAY_SIZE(volume_set_ack_false));
}

void clean_voluem_table_after_sync(DbConnection dbc, int volid)
{
	exec_sql_for_volume(dbc, volid, volume_clear_non_ack, ARRAY_SIZE(volume_clear_non_ack));
}
