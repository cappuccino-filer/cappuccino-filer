#include "syncer.h"
#include <string.h>
#include <fcntl.h>
#include <soci/soci.h>

using std::string;
using std::to_string;
using soci::use;

Syncer::Syncer(DbConnection dbc, int volid)
	:dbc_(dbc)
{
	memset(&fstat_, 0, sizeof(fstat_));

	string stmt("INSERT INTO vol_");
	stmt += to_string(volid);
	stmt += "_dentry_table";
	stmt += " (dnode, name, inode, ack) VALUES (:d, :n, :i, 1) ON DUPLICATE KEY UPDATE inode=VALUES(inode), ack=1 ;";
	st_dentry_ = std::make_unique<soci::statement>(
			(dbc->prepare << stmt,
			 use(d_ino_),
			 use(d_name_),
			 use(st_ino_))
			);

	stmt = "INSERT INTO vol_" + to_string(volid) + "_inode_table";
	stmt += " (inode, size, mtime_sec, mtime_nsec, ack) VALUES (:1, :2, :3, :4, 1) ON DUPLICATE KEY UPDATE size=VALUES(size), mtime_sec=VALUES(mtime_sec), mtime_nsec=VALUES(mtime_nsec), ack=1 ;";
	st_inode_ = std::make_unique<soci::statement>(
			(dbc->prepare << stmt,
			use(fstat_.st_ino),
			use(fstat_.st_size),
			use(fstat_.st_mtim.tv_sec),
			use(fstat_.st_mtim.tv_nsec))
			);
}

Syncer::~Syncer()
{
}

/*
 * Update tables here
 * 	1. dentry table
 * 		+ 
 * 	2. inode table
 * 		+ prepare statements with dnode_ and fstat_
 * 		+ update dnode_ and fstat_ accordingly
 * 		+ call soci::statements::execute
 */

void Syncer::sync_cwd(std::vector<std::string>& subdirs)
{
	DIR* dir = opendir(".");
	struct stat dirstat;
	::fstat(dirfd(dir), &dirstat);

	struct dirent* presult;
	while (presult = ::readdir(dir)) {
		if (presult->d_type != DT_UNKNOWN) {
			// Quick check
			if (presult->d_type != DT_REG &&
			    presult->d_type != DT_DIR)
				continue;
		}
		if (presult->d_name[0] == '.') {
			if (presult->d_name[1] == '\0')
				continue;
			if (presult->d_name[1] == '.')
				if (presult->d_name[2] == '\0')
					continue;
		}
		::fstatat(dirfd(dir), presult->d_name, &fstat_, AT_NO_AUTOMOUNT|AT_SYMLINK_NOFOLLOW);

		st_inode_->execute(true);
		d_ino_ = (uint64_t)dirstat.st_ino;
		st_ino_ = (uint64_t)presult->d_ino;
		d_name_ = presult->d_name;
		st_dentry_->execute(true);

		if (S_ISDIR(fstat_.st_mode)) {
			if (fstat_.st_dev == dirstat.st_dev)
				subdirs.emplace_back(presult->d_name);
		}
	}
	closedir(dir);
}
