#ifndef STORAGE_FILESTAT_H
#define STORAGE_FILESTAT_H

#include "decl.h"
#include <json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

class FileStat {
public:
	static FileStat create(DatabasePtr, const string&);
	static FileStat create(DatabasePtr, DIR*, const string&);

	shared_ptree mkptree();

	bool is_dir() const;
private:
	FileStat(); // only create can call this
	struct stat stat_;
};

#endif
