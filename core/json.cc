#include "json.h"

shared_ptree json_mkerror(const std::string& message)
{
	auto pt = std::make_shared<ptree>();
	pt->put("class", "error");
	pt->put("message", message);
	return pt;
}
