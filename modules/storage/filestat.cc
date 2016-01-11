#include "filestat.h"
#include <unistd.h>
#include <time.h>
#include <util.h>

FileStat::FileStat()
{
}

FileStat FileStat::create(DatabasePtr, const string& path)
{
	FileStat ret;
	int c = ::lstat(path.c_str(), &ret.stat_);
	if (c < 0) {
		throw  sys_get_error_string();
	}
	return ret;
}

FileStat FileStat::create(DatabasePtr, DIR* dir, const string& path)
{
	FileStat ret;
	int c = ::fstatat(dirfd(dir),
			path.c_str(),
			&ret.stat_,
			AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW);
	if (c < 0) {
		throw  sys_get_error_string();
	}
	return ret;
}

shared_ptree FileStat::mkptree()
{
	auto pt = std::make_shared<ptree>();
	pt->put("class", "fstat");
	pt->put("inode", (uintmax_t)stat_.st_ino);
	pt->put("mode", (uintmax_t)stat_.st_mode);
	pt->put("user", (uintmax_t)stat_.st_uid);
	pt->put("group", (uintmax_t)stat_.st_gid);
	pt->put("size", (uintmax_t)stat_.st_size);
	pt->put("mtime", ctime(&stat_.st_mtime));
	pt->put("directory", is_dir());

	return pt;
}

bool FileStat::is_dir() const
{
	return !!(stat_.st_mode & S_IFDIR);
}
