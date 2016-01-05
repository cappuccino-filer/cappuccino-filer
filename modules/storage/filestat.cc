#include "filestat.h"
#include <unistd.h>
#include <time.h>
#include <util.h>

FileStat FileStat::create(DatabasePtr, const string& path)
{
	FileStat ret;
	int c = ::stat(path.c_str(), &ret.stat_);
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
	pt->put("size", (uintmax_t)stat_.st_size);
	pt->put("mtime", ctime(&stat_.st_mtime));
	pt->put("directory", !!(stat_.st_mode & S_IFDIR));

	return pt;
}
