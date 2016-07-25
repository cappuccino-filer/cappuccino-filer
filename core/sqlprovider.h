#ifndef SQL_PROVIDER_H
#define SQL_PROVIDER_H

#include <string>
#include <map>
#include <stdint.h>

class SQLProvider {
public:
	virtual ~SQLProvider();

	std::string query(int32_t cat, int32_t func) const; // Returns corresponding SQL string, this is sufficient for most cases
	std::string query_volume(int vol, int func) const; // Return the SQL string for the specified volume.
protected:
	std::map<uint64_t, std::string> sqls_;
};

#define BLEND_CAT_FUNC(cat, func) ((((uint64_t)cat) << 32) | func)
#define SQLINDEX(name_space, func) (BLEND_CAT_FUNC(name_space::cat_id, name_space::func))

#endif
