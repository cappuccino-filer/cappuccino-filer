#include "tag.h"
#include <unordered_map>
#include <string>
#include <functional>

namespace {
	typedef std::unique_ptr<TagAction> ActionPtr;
	typedef std::function<ActionPtr(shared_ptree)> fab_function_t;

#if 0 // Disable them first.
#define NAIVE_FAB(T) [](shared_ptree pt)->ActionPtr { return std::make_shared<T>(pt); }
	
	std::unordered_map<std::string, fab_function_t> fabs = {
		{"create", NAIVE_FAB(CreateTag) },
		{"name2id", NAIVE_FAB(LocateTag) },
		{"tagtag", NAIVE_FAB(TagAnotherTag) },
		{"tagrel", NAIVE_FAB(TagRelation) },
	};

#undef NAIVE_FAB
#endif

};

TagAction::TagAction(shared_ptree pt)
	:pt_(pt)
{
}

TagAction::~TagAction()
{
}

class CreateTag : public TagAction {
	CreateTag(shared_ptree pt)
		:TagAction(pt)
	{
	}

	virtual shared_ptree act() override
	{
	}
};

std::unique_ptr<TagAction> TagActionFab::fab(shared_ptree pt)
{
}
