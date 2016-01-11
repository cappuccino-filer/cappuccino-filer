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
	while (!readdir_r(dir_, (dirent*)entryp_.get(), &presult)) {
		if (!presult)
			break;
		auto finfo = FileStat::create(db_, dir_, presult->d_name);
		auto subtree = finfo.mkptree();
		subtree->put("name", presult->d_name);
		content.push_back(std::make_pair("", *subtree));
	}
	cache_->add_child("content", content);
}

shared_ptree ReadDir::mkptree()
{
	return cache_;
}
