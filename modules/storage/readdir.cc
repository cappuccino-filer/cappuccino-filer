#include "readdir.h"
#include "unistd.h"
#include "filestat.h"
#include <util.h>

ReadDir::~ReadDir()
{
	::closedir(dir_);
}

ReadDir::ReadDir(DIR* dir, DatabasePtr db)
	:dir_(dir), db_(db)
{
	int name_max = fpathconf(dirfd(dir), _PC_NAME_MAX);
	if (name_max == -1)         /* Limit not defined, or error */
			name_max = 255;         /* Take a guess */
	int dentsize = offsetof(struct dirent, d_name) + name_max + 1;
	entryp_.reset(new char[dentsize]);
}

std::unique_ptr<ReadDir> ReadDir::create(DatabasePtr db, const string& path)
{
	DIR* dir = ::opendir(path.c_str());
	if (!dir)
		throw sys_get_error_string();
	return std::unique_ptr<ReadDir>(new ReadDir(dir, db));
}

void ReadDir::refresh()
{
	cache_ = std::make_shared<ptree>();

	cache_->put("class", "ls");
	ptree content;

	struct dirent *presult;
	while (presult = readdir(dir_)) {
		if (presult->d_name[0] == '.') {
			if (presult->d_name[1] == '\0')
				continue;
			if (presult->d_name[1] == '.')
				if (presult->d_name[2] == '\0')
					continue;
		}
		auto finfo = FileStat::create(db_, dir_, presult->d_name);
		auto subtree = finfo.mkptree();
		subtree->put("name", presult->d_name);
		content.push_back(std::make_pair("", *subtree));
	}
	cache_->add_child("content", content);
}

void ReadDir::sync_to_db(DatabasePtr db,
		TabDentries& tbl,
		TabInodes& itbl,
		std::vector<std::string>& subdirs
		)
{
	/* Read the inode of "." */
	struct stat dirstat;
	::fstat(dirfd(dir_), &dirstat);
	inode_type dnode = dirstat.st_ino;

	struct dirent *presult;
	while (presult = readdir(dir_)) {
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

shared_ptree ReadDir::mkptree()
{
	return cache_;
}
