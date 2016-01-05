#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <string>

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;
typedef std::shared_ptr<ptree> shared_ptree;

shared_ptree json_mkerror(const std::string&);

#endif
