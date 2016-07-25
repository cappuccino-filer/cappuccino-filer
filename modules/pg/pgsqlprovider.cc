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
			SQLINDEX(volume, CREATE),
R"zzz(
CREATE TABLE IF NOT EXISTS vol_#id_inode_table (inode BIGINT PRIMARY KEY, size
BIGINT NOT NULL, hash BLOB(32) NULL, mtime_sec BIGINT NULL, mtime_nsec BIGINT
NULL, last_check DATETIME NULL, ack BOOLEAN);
CREATE UNIQUE INDEX idx_inode ON vol_#id_inode_table (inode);
CREATE TABLE IF NOT EXISTS vol_#id_dentry_table (dnode BIGINT NOT NULL, name VARCHAR(255) NOT NULL, inode BIGINT NOT NULL, ack BOOLEAN, PRIMARY KEY (dnode, name) );
CREATE UNIQUE INDEX idx_dentry ON vol_#id_dentry_table (dnode, name);
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
1) ON CONFLICT ON CONFLICT vol_#id_dentry_table_pkey DO UPDATE SET inode=EXCLUDED.inode, ack=1;
)zzz"
		},
		{
			SQLINDEX(volume, UPSERT_INODE),
R"zzz(
INSERT INTO vol_#id_inode_table (inode, size, mtime_sec, mtime_nsec, ack)
VALUES (:1, :2, :3, :4, 1) ON CONFLICT ON CONFLICT vol_#id_inode_table_pkey DO UPDATE SET size=EXCLUDED.size,
mtime_sec=EXCLUDED.mtime_sec, mtime_nsec=EXCLUDED.mtime_nsec, ack=1;
)zzz"
		},
	};
}
