#ifndef STORAGE_FILESTAT_H
#define STORAGE_FILESTAT_H

#include "decl.h"
#include <json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "tbl_files.h"

typedef uint64_t inode_type;

class FileStat {
public:
	static FileStat create(DatabasePtr, const string&);
	static FileStat create(DatabasePtr, DIR*, const string&);

	shared_ptree mkptree();

	bool is_dir() const;
	bool is_symlink() const;
	bool is_special() const;
	void sync_to_db(DatabasePtr db,
			TabInodes&);
	inode_type get_inode() const;
private:
	FileStat(); // only create can call this
	struct stat stat_;
};

#endif
