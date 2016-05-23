#ifndef STORAGE_SYNCER_H
#define STORAGE_SYNCER_H

#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include "decl.h"

namespace soci {
	class statement;
};

class Syncer {
public:
	Syncer(DbConnection, int volid);
	~Syncer();

	void sync_cwd(std::vector<std::string>& subdirs);
private:
	DbConnection dbc_;
	std::unique_ptr<soci::statement> st_dentry_, st_inode_;

	struct stat fstat_;
	uint64_t d_ino_, st_ino_;
	std::string d_name_;
};

#endif
