#include "filestat.h"
#include <unistd.h>
#include <time.h>
#include <util.h>

FileStat::FileStat()
{
}

FileStat FileStat::create(DbConnection, const string& path)
{
	FileStat ret;
	int c = ::lstat(path.c_str(), &ret.stat_);
	if (c < 0) {
		throw  sys_get_error_string();
	}
	return ret;
}

FileStat FileStat::create(DbConnection, DIR* dir, const string& path)
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
	ptree pt;
	pt.put("class", "fstat");
	pt.put("inode", (uintmax_t)stat_.st_ino);
	pt.put("mode", (uintmax_t)stat_.st_mode);
	pt.put("user", (uintmax_t)stat_.st_uid);
	pt.put("group", (uintmax_t)stat_.st_gid);
	pt.put("size", (uintmax_t)stat_.st_size);
	pt.put("mtime", ctime(&stat_.st_mtime));
	pt.put("directory", is_dir());

	return pt;
}

bool FileStat::is_dir() const
{
	return S_ISDIR(stat_.st_mode);
}

bool FileStat::is_symlink() const
{
	return S_ISLNK(stat_.st_mode);
}

bool FileStat::is_special() const
{
	return !(S_ISREG(stat_.st_mode)||S_ISDIR(stat_.st_mode));
}

#if 0
void FileStat::sync_to_db(DbConnection db, TabInodes& tbl)
{
	auto selold = dynamic_select(*db)
		.dynamic_columns(all_of(tbl))
		.dynamic_from(tbl)
		.dynamic_where(tbl.inode == long(stat_.st_ino));
	bool changed = true;
	for(const auto& old : db->run(selold)) {
		// TODO: compare mtime
		changed = false;
		if (old.size != stat_.st_size)
			changed = true;
		break;
	}
	qDebug() << "Checking inode " << stat_.st_ino << " changed: " << changed;

	// TODO: set mtime
	db->run(dynamic_insert_into(*db, tbl).set_or_update(
			1, // Skip the first one which is Primary key
			tbl.inode = long(stat_.st_ino),
			tbl.size = stat_.st_size,
			//tbl.mtime = stat_.st_mtime,
			tbl.ack = true
			));
#if 0
	if (changed)
		dynamic_insert_into(*db, tbl).dynamic_set(
				tbl.hash = sqlpp::null,
				tbl.inode = long(stat_.st_ino)
				);
#endif
}
#endif

uint64_t FileStat::get_inode() const
{
	return stat_.st_ino;
}
