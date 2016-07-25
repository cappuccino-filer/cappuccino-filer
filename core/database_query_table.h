#ifndef DATABASE_QUERY_TABLE_H
#define DATABASE_QUERY_TABLE_H

namespace query {
	enum {
		CAT_META,
		CAT_VOLUME,
	};

	namespace meta {
		constexpr int cat_id = CAT_META;
		enum {
			CREATE_VOLUME_RECORD_TABLE,
			UPSERT_SEEN_VOLUME,
			LIST_SEEN_VOLUMES,
			UPSERT_TRACKING_RECORD,
			ADD_NEW_TRACKING_WITH_RETURN,
		};
	};

	namespace volume {
		constexpr int cat_id = CAT_VOLUME;
		enum {
			CREATE,
			SYNC_INIT_ACK,
			SYNC_CLEAR_NON_ACK,
			UPSERT_DENTRY,
			UPSERT_INODE,
		};
	};
};

#endif
