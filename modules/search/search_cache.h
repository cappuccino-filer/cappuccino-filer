#ifndef SEARCH_CACHE_H
#define SEARCH_CACHE_H

#include <memory>
#include <vector>
#include <chrono>
#include <json.h>
#include <uint256.h>
#include <mutex>

class Searcher;
struct CacheLine;

class SearchCache {
public:
	SearchCache();
	~SearchCache();

	std::shared_ptr<Searcher> find(ptree pt);
	uint256_t cache(std::shared_ptr<Searcher>);
	bool keep_alive(const std::string&); // return false if not found
	void expunge_outdated_lines();
	size_t size() { return cache_.size(); }
private:
	std::vector<std::unique_ptr<CacheLine>> cache_;
	std::mutex mutex_; // FIXME: consider upgradable RW lock
};

#endif
