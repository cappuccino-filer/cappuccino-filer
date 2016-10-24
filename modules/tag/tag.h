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
	void render_fail(ptree& ret, const char* reason);
	void render_ok(ptree& ret);
};

class TagActionFab {
public:
	static std::unique_ptr<TagAction> fab(shared_ptree);
};

#endif
