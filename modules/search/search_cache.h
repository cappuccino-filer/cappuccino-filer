#ifndef SEARCH_CACHE_H
#define SEARCH_CACHE_H

#include <memory>
#include <vector>
#include <chrono>
#include <json.h>
#include <uint256.h>

class Searcher;
struct CacheLine;

class SearchCache {
public:
	SearchCache();
	~SearchCache();

	std::shared_ptr<Searcher> find(ptree pt);
	uint256_t cache(std::shared_ptr<Searcher>);
	void expunge_outdated_lines();
private:
	std::vector<std::unique_ptr<CacheLine>> cache_;
};

#endif
