#include "sqlprovider.h"
#include "database_query_table.h"
#include <boost/algorithm/string.hpp>
#include <sstream>


SQLProvider::~SQLProvider()
{
}

std::string SQLProvider::query(int32_t cat, int32_t func) const
{
	auto iter = sqls_.find(BLEND_CAT_FUNC(cat, func));
	if (iter == sqls_.end()) {
		throw std::runtime_error("SQL Provider does not implement "+std::to_string(cat)+":"+std::to_string(func));
	}
	return iter->second;
}

std::string SQLProvider::query_volume(int vol, int func) const
{
	auto sql = query(query::volume::cat_id, func);
	boost::replace_all(sql, "#id", std::to_string(vol));
	return sql;
}

std::string SQLProvider::query_where_in(int32_t cat, int32_t func, const std::set<int>& set) const
{
	auto sql = query(query::volume::cat_id, func);
	std::ostringstream ss;
	ss << '(';
	const char* separator = "";
	for (auto v : set) {
		ss << separator << v;
		separator = ",";
	}
	ss << ')';
	boost::replace_all(sql, "#set", ss.str());
	return sql;
}
