#ifndef STORAGE_FILESTAT_H
#define STORAGE_FILESTAT_H

#include "decl.h"
#include <json.h>
#include <sys/types.h>
#include <sys/stat.h>

class FileStat {
public:
	static FileStat create(DatabasePtr, const string&);

	shared_ptree mkptree();
private:
	struct stat stat_;
};

#endif
