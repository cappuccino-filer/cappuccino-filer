#ifndef TAG_FILETAG_H
#define TAG_FILETAG_H

#include <json.h>
#include "tag.h"

class FileTagActionFab {
public:
	static std::unique_ptr<TagAction> fab(shared_ptree);
};


#endif
