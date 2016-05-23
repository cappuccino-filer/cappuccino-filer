#ifndef STORAGE_FILESTAT_H
#define STORAGE_FILESTAT_H

#include "decl.h"
#include <fcntl.h>
#include <json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef uint64_t inode_type;

class FileStat {
public:
	static FileStat create(DbConnection, const string&);
	static FileStat create(DbConnection, DIR*, const string&);

	shared_ptree mkptree();

	bool is_dir() const;
	bool is_symlink() const;
	bool is_special() const;
#if 0
	void sync_to_db(DbConnection db,
			TabInodes&);
#endif
	inode_type get_inode() const;
private:
	FileStat(); // only create can call this
	struct stat stat_;
};

#endif
