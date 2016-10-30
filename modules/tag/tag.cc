#include "tag.h"
#include <soci/soci.h>
#include <database.h>
#include <database_query_table.h>
#include <pref.h>
#include <unordered_map>
#include <string>
#include <functional>

using std::string;

TagAction::TagAction(shared_ptree pt)
	:pt_(pt)
{
}

TagAction::~TagAction()
{
}

void TagAction::render_fail(ptree& ret, const char* reason)
{
	ret.put("cat", pt_.get("cat", ""));
	ret.put("result", "Error");
	ret.put("reason", reason);
}

void TagAction::render_ok(ptree& ret)
{
	ret.put("cat", pt_.get("cat", ""));
	ret.put("result", "OK");
}

class DummyTagAction : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		render_fail(ret, "Unsupported tag action category");
		string oldjson;
		pt_.dump_to(oldjson);
		ret.put("request", std::move(oldjson));
		return ret;
	}
};

class CreateTag : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		string name = pt_.get("name", "");
		if (name.empty()) {
			render_fail(ret, "Tag name is empty");
			return ret;
		}

		auto dbc = DatabaseRegistry::get_shared_dbc();
		int tid;
		*dbc << RETRIVE_SQL_QUERY(query::tag, CREATE),
			soci::into(tid),
			soci::use(name);
		render_ok(ret);
		ret.put("name", name);
		ret.put("tagid", tid);
		return ret;
	}
};

class ListTag : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		int offset;
		int limit;
		offset = pt_.get("offset", -1);
		limit = pt_.get("limit", -1);
		int limit_cap = Pref::instance()->get_registry().get("tagger.list.limits", 100);

		if (offset < 0) {
			render_fail(ret, "Invalid offset");
			return ret;
		} else if (limit < 0) {
			render_fail(ret, "Invalid limit");
			return ret;
		}
		limit = std::min(limit, limit_cap);

		auto dbc = DatabaseRegistry::get_shared_dbc();
		soci::rowset<soci::row> taglist = (dbc->prepare <<
				RETRIVE_SQL_QUERY(query::tag, LIST),
				soci::use(offset),
				soci::use(limit));
		render_ok(ret);
		ptree array;
		for (const auto& row: taglist) {
			ptree tag;
			tag.put("id", row.get<int>(0));
			tag.put("name", row.get<string>(1));
			array.push_back(std::move(tag));
		}
		ret.swap_child_with("tags", array);
		return ret;
	}
};

namespace {
	typedef std::unique_ptr<TagAction> ActionPtr;
	typedef std::function<ActionPtr(shared_ptree)> fab_function_t;

#define NAIVE_FAB(T) [](shared_ptree pt)->ActionPtr { return std::make_unique<T>(pt); }

	std::unordered_map<string, fab_function_t> fabmap = {
		{"create", NAIVE_FAB(CreateTag) },
		{"list", NAIVE_FAB(ListTag) }
#if 0 // Disable them first.
		, {"name2id", NAIVE_FAB(LocateTag) }
		, {"tagtag", NAIVE_FAB(TagAnotherTag) }
		, {"tagrel", NAIVE_FAB(TagRelation) }
#endif
	};

#undef NAIVE_FAB

};

std::unique_ptr<TagAction> TagActionFab::fab(shared_ptree pt)
{
	string cat = pt.get("cat", "");
	auto iter = fabmap.find(cat);
	if (iter == fabmap.end())
		return std::make_unique<DummyTagAction>(pt);
	return iter->second(pt);
}
