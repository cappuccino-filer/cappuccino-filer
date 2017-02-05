#ifndef DATABASE_QUERY_TABLE_H
#define DATABASE_QUERY_TABLE_H

namespace query {
	enum {
		CAT_META,
		CAT_VOLUME,
		CAT_TAG,
		CAT_FILE_TAG,
	};

	namespace meta {
		constexpr int cat_id = CAT_META;
		enum {
			CREATE_VOLUME_RECORD_TABLE,
			UPSERT_SEEN_VOLUME,
			LIST_SEEN_VOLUMES,
			UPSERT_TRACKING_RECORD,
			ADD_NEW_TRACKING_WITH_RETURN,
			DROP_ALL_TABLES,
		};
	};

	namespace volume {
		constexpr int cat_id = CAT_VOLUME;
		enum {
			CREATE,
			UPDATE_ROOT_INODE,
			SYNC_INIT_ACK,
			SYNC_CLEAR_NON_ACK,
			UPSERT_DENTRY,
			UPSERT_INODE,
			REGEX_NAME_MATCH,
			ASSIGN_TAG
		};
	};

	namespace tag {
		constexpr int cat_id = CAT_TAG;
		enum {
			TABLE_CREATION,
			FUNCTION_CREATION,
			CREATE,
			LIST,
			NAME_TO_ID,
			NAME_TO_ID_EX,
			DELETE,
			FIND_TAG_TAG_RELATION,
			UPSERT_TAG_TAG_RELATION,
			FIND_REL_TAG_RELATION,
			UPSERT_REL_TAG_RELATION,
		};
	};
};

#endif
