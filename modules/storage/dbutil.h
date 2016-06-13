#ifndef DBUTIL_H
#define DBUTIL_H

#include <database.h>

void create_volume_table(DbConnection dbc, int volid);
void set_voluem_table_before_sync(DbConnection dbc, int volid);
void clean_voluem_table_after_sync(DbConnection dbc, int volid);

#endif
