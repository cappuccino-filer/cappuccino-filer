#include "search_cache.h"

static bool cache_rcu_compare(const CacheLine& lhs, const CacheLine& rhs)
{
	return lhs.recent_update_time < rhs.recent_update_time;
}

std::shared_ptr<Searcher>
SearchCache::find(ptree pt)
{
	std::string cookie = pt.get("cache-cookie", "0x0");
	uint256_t key;
	hex_to_uint256(cookie, key);
	for (size_t i = 0; i < cache_.size(); i++) {
		auto& line = cache_[i];
		if (line->key == key) {
			auto ret = line.value;
			line.refresh();
			cache_.back().swap(line);
			return ret;
		}
	}
	return nullptr;
}

uint256_t
SearchCache::cache(std::shared_ptr<Searcher> value)
{
	auto line = std::make_shared<CacheLine>;
	uint256_t ret = uint256_gen_random();
	line->key = ret;
	line->value = value;
	line->refresh();
	cache_.emplace_back(std::move(line));
	return ret;
}
