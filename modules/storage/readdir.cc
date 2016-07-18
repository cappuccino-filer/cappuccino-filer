#include "readdir.h"
#include "unistd.h"
#include "filestat.h"
#include <util.h>

ReadDir::~ReadDir()
{
	::closedir(dir_);
}

ReadDir::ReadDir(DIR* dir, DbConnection db)
	:dir_(dir), db_(db)
{
}

std::unique_ptr<ReadDir> ReadDir::create(DbConnection db, const string& path)
{
	DIR* dir = ::opendir(path.c_str());
	if (!dir)
		throw sys_get_error_string();
	return std::unique_ptr<ReadDir>(new ReadDir(dir, db));
}

void ReadDir::refresh()
{
	cache_ = ptree::create();

	cache_.put("class", "ls");
	ptree content;

	struct dirent *presult;
	while (presult = ::readdir(dir_)) {
		if (presult->d_name[0] == '.') {
			if (presult->d_name[1] == '\0')
				continue;
			if (presult->d_name[1] == '.')
				if (presult->d_name[2] == '\0')
					continue;
		}
		auto finfo = FileStat::create(db_, dir_, presult->d_name);
		auto subtree = finfo.mkptree();
		subtree.put("name", presult->d_name); // FIXME
		content.push_back(subtree);
	}
	cache_.put("content", content);
}

#if 0
void ReadDir::sync_to_db(DbConnection db,
		Syncer& syncer,
		std::vector<std::string>& subdirs
		)
{
	/* Read the inode of "." */
	struct stat dirstat;
	::fstat(dirfd(dir_), &dirstat);
	inode_type dnode = dirstat.st_ino;

	while (presult = ::readdir(dir_)) {
		auto finfo = FileStat::create(db, dir_, presult->d_name);
		if (finfo.is_special())
			continue;
		if (presult->d_name[0] == '.') {
			if (presult->d_name[1] == '\0')
				continue;
			if (presult->d_name[1] == '.')
				if (presult->d_name[2] == '\0')
					continue;
		}
		finfo.sync_to_db(db, itbl);
		db->run(dynamic_insert_into(*db, tbl)
			.set_or_update(
				1,
				tbl.inode = long(finfo.get_inode()),
				tbl.ack = true,
				tbl.dnode = long(dnode),
				tbl.name = presult->d_name
			));
		if (finfo.is_dir())
			subdirs.emplace_back(presult->d_name);
	}
}
#endif

shared_ptree ReadDir::mkptree()
{
	return cache_;
}
