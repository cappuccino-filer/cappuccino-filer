#ifndef STORAGE_READDIR_H
#define STORAGE_READDIR_H

#include "decl.h"
#include <json.h>
#include <dirent.h>

class ReadDir {
public:
	~ReadDir();
	static std::unique_ptr<ReadDir> create(DbConnection, const string&);

	void refresh();
	shared_ptree mkptree();

	bool is_dir() const;
#if 0
	void sync_to_db(DbConnection db,
			TabDentries&,
			TabInodes&,
			std::vector<std::string>& subdirs
			);
#endif
private:
	ReadDir(DIR* dir, DbConnection db); // only create can call this
	DbConnection db_;
	DIR* dir_;
	shared_ptree cache_;
};

#endif
