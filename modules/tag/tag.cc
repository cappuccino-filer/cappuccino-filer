#include "tag.h"
#include "filetag.h"
#include <soci/soci.h>
#include <database.h>
#include <database_query_table.h>
#include <pref.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <set>
#include <QDebug>

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

#include "tag_actions.h"
#include "filetag_actions.h"

namespace {
	typedef std::unique_ptr<TagAction> ActionPtr;
	typedef std::function<ActionPtr(shared_ptree)> fab_function_t;

#define NAIVE_FAB(T) [](shared_ptree pt)->ActionPtr { return std::make_unique<T>(pt); }

	std::unordered_map<string, fab_function_t> tagfabmap = {
		{"create", NAIVE_FAB(CreateTag) }
		, {"list", NAIVE_FAB(ListTag) }
		, {"name2id", NAIVE_FAB(LocateTag) }
		, {"tagtag", NAIVE_FAB(TagAnotherTag) }
		, {"delete_tag", NAIVE_FAB(DeleteTags) }
#if 0 // Disable them first.
		, {"tagrel", NAIVE_FAB(TagRelation) }
#endif
	};

	std::unordered_map<string, fab_function_t> filetagfabmap = {
		{"assign", NAIVE_FAB(AssigTagToFile) }
#if 0 // Disable them first.
		, {"tagrel", NAIVE_FAB(TagRelation) }
#endif
	};

#undef NAIVE_FAB

	std::unique_ptr<TagAction>
	find_and_construct(const std::unordered_map<string, fab_function_t>& fabmap,
			   ptree pt)
	{
		string cat = pt.get("cat", "");
		auto iter = fabmap.find(cat);
		if (iter == fabmap.end())
			return std::make_unique<DummyTagAction>(pt);
		else
			return iter->second(pt);
	}
};

std::unique_ptr<TagAction> TagActionFab::fab(shared_ptree pt)
{
	return find_and_construct(tagfabmap, pt);
}

std::unique_ptr<TagAction> FileTagActionFab::fab(shared_ptree pt)
{
	return find_and_construct(filetagfabmap, pt);
}
