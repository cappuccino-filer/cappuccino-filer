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
}
