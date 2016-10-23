#ifndef TAG_TAG_H
#define TAG_TAG_H

#include <json.h>

class TagAction {
public:
	TagAction(shared_ptree);
	~TagAction();

	virtual shared_ptree act() = 0;
protected:
	const shared_ptree pt_;
};

class TagActionFab {
public:
	static std::unique_ptr<TagAction> fab(shared_ptree);
};

#endif
