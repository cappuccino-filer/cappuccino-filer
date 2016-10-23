{
	SQLINDEX(volume, CREATE),
R"zzz(
CREATE TABLE IF NOT EXISTS vol_#id_inode_table (inode BIGINT PRIMARY KEY, size
BIGINT NOT NULL, hash BYTEA NULL, mtime_sec BIGINT NULL, mtime_nsec BIGINT
NULL, last_check TIMESTAMP WITH TIME ZONE NULL, ack BOOLEAN);
CREATE UNIQUE INDEX IF NOT EXISTS vol_#id_idx_inode ON vol_#id_inode_table (inode);
CREATE TABLE IF NOT EXISTS vol_#id_dentry_table (dnode BIGINT NOT NULL, name VARCHAR(255) NOT NULL, inode BIGINT NOT NULL, ack BOOLEAN, PRIMARY KEY (dnode, name) );
CREATE UNIQUE INDEX IF NOT EXISTS vol_#id_idx_dentry ON vol_#id_dentry_table (dnode, name);
CREATE TABLE IF NOT EXISTS vol_#id_file_tag_relation (relid BIGSERIAL PRIMARY KEY);
CREATE TABLE IF NOT EXISTS vol_#id_file_tag_first_order (
	relid BIGINT REFERENCES vol_#id_file_tag_relation (relid) ON DELETE CASCADE,
	taggee BIGINT REFERENCES vol_#id_inode_table (inode) ON DELETE CASCADE,
	tagger INTEGER REFERENCES tag_table (id) ON DELETE CASCADE,
	p REAL DEFAULT 1.0,
	PRIMARY KEY (taggee, tagger) );
CREATE TABLE IF NOT EXISTS vol_#id_file_tag_higher_order (
	relid BIGINT REFERENCES vol_#id_file_tag_relation (relid) ON DELETE CASCADE,
	taggee BIGINT REFERENCES vol_#id_file_tag_relation (relid) ON DELETE CASCADE,
	tagger INTEGER REFERENCES tag_table(id) ON DELETE CASCADE,
	p REAL DEFAULT 1.0,
	PRIMARY KEY (taggee, tagger) );
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
((SELECT name, dnode, inode, CAST('' AS text) AS path
FROM vol_#id_dentry_table
WHERE name ~ :p
ORDER BY dnode ASC, inode ASC
OFFSET :off
LIMIT :lim
)
UNION ALL
SELECT fstree.name, dt.dnode, fstree.inode, CAST('/' || dt.name || fstree.path AS text) AS path
FROM vol_#id_dentry_table AS dt
     INNER JOIN fstree
     ON (dt.inode = fstree.dnode)
)
SELECT name,path FROM fstree WHERE dnode=(SELECT root_inode FROM tracking_table WHERE trID=#id);
)zzz"
}
