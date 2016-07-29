#ifndef SEARCHER_H
#define SEARCHER_H

#include <json.h>
#include <memory>

class Searcher;

class SearcherFab {
public:
	static std::unique_ptr<Searcher> fab(ptree);
};

class Searcher {
public:
	Searcher(ptree r);
	~Searcher();

	virtual ptree do_search() = 0;
protected:
	ptree req_;
	ptree ans_;

	template<typename T>
	T get(const std::string& path, const T& def)
	{
		return req_.get(path, def);
	}
};

#endif
