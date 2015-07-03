#include <vector>
#include <unordered_map>
#include <thread>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include "pipeline.h"

#define GUARD_CHANGES(mutex) boost::shared_lock<boost::shared_mutex> lock(mutex)
#define MAYDO_CHANGES(mutex) boost::upgrade_lock<boost::shared_mutex> lock(mutex)
#define WILLDO_CHANGES(mutex) boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock)
#define JUSTDO_CHANGES(mutex) boost::unique_lock< boost::shared_mutex > lock(mutex)

namespace pipeline {
	using std::string;
	typedef std::vector<ParcelHandler*> ParcelHandlerList;

	boost::shared_mutex pipeline_mutex;
	int classid = 0;
#define GUARD_PIPELINE_CHANGES GUARD_CHANGES(pipeline_mutex)
#define MAYDO_PIPELINE_CHANGES MAYDO_CHANGES(pipeline_mutex)
#define WILLDO_PIPELINE_CHANGES WILLDO_CHANGES(pipeline_mutex)
#define JUSTDO_PIPELINE_CHANGES JUSTDO_CHANGES(pipeline_mutex)
	std::unordered_map<string, int> parcel_classifier;
	std::unordered_map<int, ParcelHandlerList> parcel_handlers;
	std::unordered_map<int, ParcelTranslator*> parcel_translators;

	int classify_parcel(const string& classname)
	{
		MAYDO_PIPELINE_CHANGES;
		const auto iter = parcel_classifier.find(classname);
		if (iter != parcel_classifier.end())
			return iter->second;
		WILLDO_PIPELINE_CHANGES;
		parcel_classifier[classname] = classid;
		return classid++;
	}

	void set_parcel_translator(int classid, ParcelTranslator* translator)
	{
		JUSTDO_PIPELINE_CHANGES;
		parcel_translators[classid] = translator;
	}

	ParcelTranslator* get_parcel_translator(int classid)
	{
		GUARD_PIPELINE_CHANGES;
		return parcel_translators[classid];
	}

	void add_parcel_handler(int classid, ParcelHandler* handler)
	{
		JUSTDO_PIPELINE_CHANGES;
		parcel_handlers[classid].emplace_back(handler);
	}

	void push_json(const string& json)
	{
		pt::ptree tree;
		pt::read_json(json, tree);
		string classname = tree.get<string>("class");
		int classid = classify_parcel(classname);
		auto translator = get_parcel_translator(classid);
		auto parcel = translator->parse_json(tree);
		push_parcel(parcel);
	}

	void push_parcel(ParcelPtr parcel)
	{
		GUARD_PIPELINE_CHANGES;
		for (auto handler : parcel_handlers[parcel->classid]) {
			handler->async_handle(parcel);
		}
	}
}
