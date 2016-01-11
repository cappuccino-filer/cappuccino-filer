#ifndef STORAGE_READDIR_H
#define STORAGE_READDIR_H

#include "decl.h"
#include <json.h>
#include <dirent.h>

class ReadDir {
public:
	~ReadDir();
	static std::unique_ptr<ReadDir> create(DatabasePtr, const string&);

	void refresh();
	shared_ptree mkptree();

	bool is_dir() const;
private:
	ReadDir(DIR* dir, DatabasePtr db); // only create can call this
	DatabasePtr db_;
	DIR* dir_;
	shared_ptree cache_;
	std::unique_ptr<char[]> entryp_;
};

#endif
