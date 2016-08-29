#ifndef SEARCHER_H
#define SEARCHER_H

#include <json.h>
#include <memory>

class Searcher;
class SearchCache;

class SearcherFab {
public:
	static std::shared_ptr<Searcher> fab(ptree);
private:
	static SearchCache cache_;
};

struct uint256_t;

class Searcher {
public:
	Searcher(ptree r);
	virtual ~Searcher();
	void set_unique_key(const uint256_t&);

	virtual ptree do_search() = 0;
	virtual void page(ptree); // Transfer request page # to the searcher
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
