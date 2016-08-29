#include "pgsqlprovider.h"
#include <database_query_table.h>

using namespace query;

PGProvider::PGProvider()
{
	sqls_ = {
		{
			SQLINDEX(meta, CREATE_VOLUME_RECORD_TABLE),
R"zzz(
CREATE TABLE IF NOT EXISTS volumes_table(
	uuid char(40) PRIMARY KEY,
	label char(32),
	mount text
);
CREATE TABLE IF NOT EXISTS tracking_table(
	trID SERIAL PRIMARY KEY,
	uuid char(40) UNIQUE,
	tracking int NOT NULL DEFAULT 0,
	root_inode BIGINT,
	FOREIGN KEY(uuid) REFERENCES volumes_table(uuid)
);
)zzz"
		},
		{
			SQLINDEX(meta, UPSERT_SEEN_VOLUME),
R"zzz(
INSERT INTO volumes_table(uuid, label, mount) VALUES(:1, :2, :3) ON CONFLICT ON CONSTRAINT volumes_table_pkey
DO UPDATE SET mount = EXCLUDED.mount;
)zzz"
		},
		{
			SQLINDEX(meta, LIST_SEEN_VOLUMES),
R"zzz(
SELECT volumes_table.uuid, mount, CASE WHEN tracking_table.trID IS NOT NULL
THEN tracking_table.tracking ELSE 0 END AS tracking FROM volumes_table LEFT
JOIN tracking_table ON (volumes_table.uuid = tracking_table.uuid);
)zzz"
		},
		{
			SQLINDEX(meta, UPSERT_TRACKING_RECORD),
R"zzz(
INSERT INTO tracking_table(uuid, tracking) VALUES(:1, :2) ON CONFLICT ON CONSTRAINT volumes_table_uuid_key DO
UPDATE SET tracking = EXCLUDED.tracking;
)zzz"
		},
		{
			SQLINDEX(meta, ADD_NEW_TRACKING_WITH_RETURN),
R"zzz(
INSERT INTO tracking_table(uuid,tracking) VALUES(:1,1) RETURNING trID;
)zzz"
		},
		{
			SQLINDEX(meta, DROP_ALL_TABLES),
R"zzz(
SELECT 'DROP TABLE IF EXISTS "' || tablename || '" CASCADE;' 
FROM pg_tables
WHERE schemaname = 'public';
)zzz"
		},
		{
			SQLINDEX(volume, CREATE),
R"zzz(
CREATE TABLE IF NOT EXISTS vol_#id_inode_table (inode BIGINT PRIMARY KEY, size
BIGINT NOT NULL, hash BYTEA NULL, mtime_sec BIGINT NULL, mtime_nsec BIGINT
NULL, last_check TIMESTAMP WITH TIME ZONE NULL, ack BOOLEAN);
CREATE UNIQUE INDEX vol_#id_idx_inode ON vol_#id_inode_table (inode);
CREATE TABLE IF NOT EXISTS vol_#id_dentry_table (dnode BIGINT NOT NULL, name VARCHAR(255) NOT NULL, inode BIGINT NOT NULL, ack BOOLEAN, PRIMARY KEY (dnode, name) );
CREATE UNIQUE INDEX vol_#id_idx_dentry ON vol_#id_dentry_table (dnode, name);
)zzz"
		},
		{
			SQLINDEX(volume, UPDATE_ROOT_INODE),
R"zzz(
UPDATE tracking_table SET root_inode=:1 WHERE trID=#id;
)zzz"
		},
		{
			SQLINDEX(volume, SYNC_INIT_ACK),
R"zzz(
UPDATE vol_#id_dentry_table SET ack = false;
UPDATE vol_#id_inode_table SET ack = false;
)zzz"
		},
		{
			SQLINDEX(volume, SYNC_CLEAR_NON_ACK),
R"zzz(
DELETE FROM vol_#id_dentry_table WHERE ack = false;
DELETE FROM vol_#id_inode_table WHERE ack = false;
)zzz"
		},
		{
			SQLINDEX(volume, UPSERT_DENTRY),
R"zzz(
INSERT INTO vol_#id_dentry_table (dnode, name, inode, ack) VALUES (:d, :n, :i,
true) ON CONFLICT ON CONSTRAINT vol_#id_dentry_table_pkey DO UPDATE SET inode=EXCLUDED.inode, ack = true;
)zzz"
		},
		{
			SQLINDEX(volume, UPSERT_INODE),
R"zzz(
INSERT INTO vol_#id_inode_table (inode, size, mtime_sec, mtime_nsec, ack)
VALUES (:1, :2, :3, :4, true) ON CONFLICT ON CONSTRAINT vol_#id_inode_table_pkey DO UPDATE SET size=EXCLUDED.size,
mtime_sec=EXCLUDED.mtime_sec, mtime_nsec=EXCLUDED.mtime_nsec, ack = true;
)zzz"
		},
		{
			SQLINDEX(volume, REGEX_NAME_MATCH),
R"zzz(
WITH RECURSIVE fstree AS
(SELECT name, dnode, inode, CAST('' AS text) AS path
FROM vol_#id_dentry_table
WHERE name ~ :p
ORDER BY inode
LIMIT :lim
OFFSET :off
UNION ALL
SELECT fstree.name, dt.dnode, fstree.inode, CAST('/' || dt.name || fstree.path AS text) AS path
FROM vol_#id_dentry_table AS dt
     INNER JOIN fstree
     ON (dt.inode = fstree.dnode)
)
SELECT name,path FROM fstree WHERE dnode=(SELECT root_inode FROM tracking_table WHERE trID=#id);
)zzz"
		},
	};
}
