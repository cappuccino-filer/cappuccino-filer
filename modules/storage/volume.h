#ifndef STORAGE_VOLUME_H
#define STORAGE_VOLUME_H

#include <database.h>
#include <json.h>
class Pref;

class Volume {
public:
	Volume();

	static Volume* instance();
	void scan(DbConnection);
	shared_ptree ls_volumes();
	shared_ptree handle_request(shared_ptree);
private:
};

#endif
