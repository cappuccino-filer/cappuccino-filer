#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <string>

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

typedef std::shared_ptr<boost::property_tree::ptree> shared_ptree;

const shared_ptree json_mkerror(const std::string&);

#endif
