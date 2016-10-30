{
	SQLINDEX(tag, TABLE_CREATION),
R"zzz(
CREATE TABLE IF NOT EXISTS tag_table (
	id SERIAL PRIMARY KEY,
	name TEXT
);
CREATE TABLE IF NOT EXISTS tag_relation (
	relid SERIAL PRIMARY KEY
);
CREATE TABLE IF NOT EXISTS tag_first_order_relation (
	relid INTEGER REFERENCES tag_relation (relid) ON DELETE CASCADE,
	taggee INTEGER REFERENCES tag_table(id) ON DELETE CASCADE,
	tagger INTEGER REFERENCES tag_table(id) ON DELETE CASCADE,
	p REAL DEFAULT 1.0,
	PRIMARY KEY (taggee, tagger)
);
CREATE TABLE IF NOT EXISTS tag_higher_order_relation (
	relid INTEGER REFERENCES tag_relation (relid) ON DELETE CASCADE,
	taggee INTEGER REFERENCES tag_relation (relid) ON DELETE CASCADE,
	tagger INTEGER REFERENCES tag_table (id) ON DELETE CASCADE,
	p REAL DEFAULT 1.0,
	PRIMARY KEY (taggee, tagger)
);
)zzz"
},
{
	SQLINDEX(tag, FUNCTION_CREATION),
R"zzz(
CREATE OR REPLACE FUNCTION
	tag_name_to_id(name_in TEXT, do_upsert INTEGER)
RETURNS SETOF tag_table AS
$BODY$
BEGIN
	RETURN QUERY SELECT * FROM tag_table WHERE name = name_in;
	IF do_upsert != 0 AND NOT FOUND THEN
		RETURN QUERY INSERT INTO tag_table(id, name) VALUES(DEFAULT, name_in) RETURNING *;
	END IF;
	RETURN ;
END
$BODY$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION
	construct_query_find_tag_tag_relation(taggee INTEGER, tagger INTEGER)
RETURNS SETOF tag_first_order_relation AS
$BODY$
BEGIN
	IF taggee >= 0 AND tagger >= 0 THEN
		RETURN QUERY SELECT * FROM tag_first_order_relation WHERE
		       taggee = taggee and tagger = tagger;
	ELSIF taggee < 0 THEN
		RETURN QUERY SELECT * FROM tag_first_order_relation WHERE tagger = tagger;
	ELSIF tagger < 0 THEN
		RETURN QUERY SELECT * FROM tag_first_order_relation WHERE taggee = taggee;
	END IF;
	RETURN;
END
$BODY$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION
	construct_query_find_rel_tag_relation(taggee INTEGER, tagger INTEGER)
RETURNS SETOF tag_first_order_relation AS
$BODY$
BEGIN
	IF taggee >= 0 AND tagger >= 0 THEN
		RETURN QUERY SELECT * FROM tag_higher_order_relation WHERE
		       taggee = taggee and tagger = tagger;
	ELSIF taggee < 0 THEN
		RETURN QUERY SELECT * FROM tag_higher_order_relation WHERE tagger = tagger;
	ELSIF tagger < 0 THEN
		RETURN QUERY SELECT * FROM tag_higher_order_relation WHERE taggee = taggee;
	END IF;
	RETURN;
END
$BODY$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION
	upsert_tag_tag_relation(taggee_in INTEGER, tagger_in INTEGER, p REAL)
RETURNS VOID AS
$BODY$
DECLARE
	relid_ret INTEGER;
BEGIN
	SELECT relid INTO relid_ret FROM tag_first_order_relation
		     WHERE taggee = taggee_in AND tagger = tagger_in;
	IF NOT FOUND THEN
		INSERT INTO tag_relation (relid) VALUES(DEFAULT) RETURNING relid INTO relid_ret;
	END IF;
	INSERT INTO tag_first_order_relation(relid, taggee, tagger, p)
		VALUES(relid_ret, taggee_in, tagger_in, p);
END
$BODY$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION
	upsert_rel_tag_relation(taggee_in INTEGER, tagger_in INTEGER, p REAL)
RETURNS VOID AS
$BODY$
DECLARE
	relid_ret INTEGER;
BEGIN
	SELECT relid INTO relid_ret FROM tag_higher_order_relation
		     WHERE taggee = taggee_in AND tagger = tagger_in;
	IF NOT FOUND THEN
		INSERT INTO tag_relation(relid) VALUES (DEFAULT) RETURNING relid INTO relid_ret;
	END IF;
	INSERT INTO tag_higher_order_relation(relid, taggee, tagger, p)
		VALUES(relid_ret, taggee_in, tagger_in, p);
END
$BODY$
LANGUAGE plpgsql;
)zzz"
},
{
	SQLINDEX(tag, CREATE),
R"zzz(
INSERT INTO tag_table(name) VALUES(:1) RETURNING id;
)zzz"
},
{
	SQLINDEX(tag, LIST),
R"zzz(
SELECT * FROM tag_table ORDER BY id ASC OFFSET :1 LIMIT :2;
)zzz"
},
{
	SQLINDEX(tag, NAME_TO_ID),
R"zzz(
SELECT * FROM tag_name_to_id(:1, :2);
)zzz"
},
{
	SQLINDEX(tag, NAME_TO_ID_EX),
R"zzz(
SELECT * FROM tag_name_to_id(:1, :2) INNER JOIN tag_first_order_relation
WHERE id = tag_first_order_relation.taggee
      AND
      tag_first_order_relation.taggee IN #set;
)zzz"
},
{
	SQLINDEX(tag, DELETE),
R"zzz(
DELETE FROM tag_table WHERE id = :1;
)zzz"
},
{
	SQLINDEX(tag, TAG_A_TAG),
R"zzz(
DO $$
DECLARE
	relid_ret INTEGER;
BEGIN
	INSERT INTO tag_relation(relid) VALUES(DEFAULT) RETURNING relid INTO relid_ret;
	INSERT INTO tag_first_order_relation(relid, taggee, tagger, p)
		VALUES(relid_ret, :1, :2, :3) RETURNING *;
END $$;
)zzz"
},
{
	SQLINDEX(tag, TAG_A_RELATION),
R"zzz(
DO $$
DECLARE
	relid_ret INTEGER;
BEGIN
	INSERT INTO tag_relation(relid) VALUES(DEFAULT) RETURNING relid INTO relid_ret;
	INSERT INTO tag_higher_order_relation(relid, taggee, tagger, p)
		VALUES(relid_ret, :1, :2, :3) RETURNING *;
END $$;
)zzz"
},
{
	SQLINDEX(tag, FIND_TAG_TAG_RELATION),
R"zzz(
SELECT * FROM construct_query_find_tag_tag_relation(:1, :2);
)zzz"
},
{
	SQLINDEX(tag, UPSERT_TAG_TAG_RELATION),
R"zzz(
SELECT upsert_tag_tag_relation(:1, :2, :3);
)zzz"
},
{
	SQLINDEX(tag, FIND_REL_TAG_RELATION),
R"zzz(
SELECT * FROM construct_query_find_rel_tag_relation(:1, :2);
)zzz"
},
{
	SQLINDEX(tag, UPSERT_REL_TAG_RELATION),
R"zzz(
SELECT upsert_rel_tag_relation(:1, :2, :3);
)zzz"
}
