#ifndef STORAGE_VOLUME_H
#define STORAGE_VOLUME_H

#include <database.h>
class Pref;

class Volume {
public:
	Volume();

	static Volume* instance();
	void scan(DatabasePtr);
private:
};

#endif
