#ifndef SEARCH_CACHE_H
#define SEARCH_CACHE_H

#include <memory>
#include <vector>
#include <chrono>
#include <json.h>
#include <uint256.h>

class Searcher;

class SearchCache {
public:
	std::shared_ptr<Searcher> find(ptree pt);
	uint256_t cache(std::shared_ptr<Searcher>);
	void expunge_outdated_lines();

private:
	struct CacheLine {
		uint256_t key;
		std::shared_ptr<Searcher> value;
		std::chrono::time_point<std::chrono::steady_clock> recent_update_time;
		void refresh()
		{
			recent_update_time = std::chrono::steady_clock::now();
		}
	};
	std::vector<std::unique_ptr<CacheLine>> cache_;
};

#endif
