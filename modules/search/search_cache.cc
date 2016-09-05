#include "search_cache.h"
#include <pref.h>
#include <QDebug>

struct CacheLine {
	uint256_t key;
	std::shared_ptr<Searcher> value;
	std::chrono::time_point<std::chrono::steady_clock> recent_update_time;
	void refresh()
	{
		recent_update_time = std::chrono::steady_clock::now();
	}
};

SearchCache::SearchCache()
{
}

SearchCache::~SearchCache()
{
}

namespace {
	bool cache_rcu_compare(const CacheLine& lhs, const CacheLine& rhs)
	{
		return lhs.recent_update_time < rhs.recent_update_time;
	}
}

std::shared_ptr<Searcher>
SearchCache::find(ptree pt)
{
	std::lock_guard<std::mutex> guard(mutex_);

	std::string cookie = pt.get("cache_cookie", "0x0");
	uint256_t key;
	hex_to_uint256(cookie, key);
	// FIXME: improve the algorithm
	for (size_t i = 0; i < cache_.size(); i++) {
		CacheLine* line = cache_[i].get();
		if (line->key == key) {
			auto ret = line->value;
			line->refresh();
			cache_.front().swap(cache_[i]);
			return ret;
		}
	}
	
	return nullptr;
}

uint256_t
SearchCache::cache(std::shared_ptr<Searcher> value)
{
	std::lock_guard<std::mutex> guard(mutex_);

	auto line = std::make_unique<CacheLine>();
	uint256_t ret = uint256_gen_random();
	line->key = ret;
	line->value = value;
	line->refresh();
	cache_.emplace_back(std::move(line));
	return ret;
}

bool
SearchCache::keep_alive(const std::string& cookie)
{
	std::lock_guard<std::mutex> guard(mutex_);

	// FIXME: merge the code with find()
	uint256_t key;
	hex_to_uint256(cookie, key);
	for (size_t i = 0; i < cache_.size(); i++) {
		CacheLine* line = cache_[i].get();
		if (line->key == key) {
			line->refresh();
			cache_.front().swap(cache_[i]);
			return true;
		}
	}
	return false;
}

namespace {
	bool
	is_newer_than(const std::unique_ptr<CacheLine>& lhs,
	              const std::unique_ptr<CacheLine>& rhs)
	{
		return lhs->recent_update_time > rhs->recent_update_time;
	};

	//
	// Find a first element that Prober returns false
	// 
	template<typename Iterator, typename Prober>
	Iterator
	first_false_element(Iterator begin, Iterator end, Prober p)
	{
		Iterator iter;
		typename std::iterator_traits<Iterator>::difference_type count, step;
		count = std::distance(begin, end);
		while (count > 0) {
			iter = begin; 
			step = count / 2; 
			std::advance(iter, step);
			if (p(*iter)) {
				begin = ++iter;
				count -= step + 1;
			} else {
				count = step;
			}
		}
		return begin;
	}
}

void
SearchCache::expunge_outdated_lines()
{
	std::lock_guard<std::mutex> guard(mutex_);

	auto reg = Pref::instance()->get_registry();
	ssize_t timeout = reg.get("searcher.cache_time", 0);
	ssize_t linelimit = reg.get("searcher.cache_limit", 0);
	std::sort(cache_.begin(), cache_.end(), is_newer_than);
	cache_.erase(cache_.begin() + std::min<ssize_t>(linelimit, cache_.size()),
	             cache_.end());
	auto earliest = std::chrono::steady_clock::now();
	earliest -= std::chrono::seconds(timeout);
	// Note: STL binary search sucks, no support for functors at all.
	auto iter = first_false_element(cache_.begin(), cache_.end(),
			[earliest](const std::unique_ptr<CacheLine>& item)
			{
				return item->recent_update_time > earliest;
			}
			);
	cache_.erase(iter, cache_.end());
	qDebug() << "Cache: " << cache_.size() << " lines left";
}
