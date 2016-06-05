#include "json.h"
#include <sstream>
#include <memory>

shared_ptree json_mkerror(const std::string& message)
{
	auto pt = std::make_shared<ptree>();
	pt->put("class", "error");
	pt->put("message", message);
	return pt;
}

void json_write_to_string(const shared_ptree pt, std::string& str)
{
	std::stringstream jsonstream;
	write_json(jsonstream, *pt, false);
	str = std::move(jsonstream.str());
}

void json_write_to_string(const ptree& pt, std::string& str)
{
	std::stringstream jsonstream;
	write_json(jsonstream, pt, false);
	str = std::move(jsonstream.str());
}

shared_ptree create_ptree()
{
	return std::make_shared<ptree>();
}
