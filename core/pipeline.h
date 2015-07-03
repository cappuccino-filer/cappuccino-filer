#ifndef CORE_PIPELINE_H
#define CORE_PIPELINE_H

#include <memory>
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace pipeline {
	namespace pt = boost::property_tree;
	struct Parcel {
		int classid;
	};
	typedef std::shared_ptr<Parcel> ParcelPtr;
	struct ParcelHandler {
		virtual void async_handle(ParcelPtr) = 0;
	};
	struct ParcelTranslator {
		virtual ParcelPtr parse_json(const pt::ptree&);
	};

	struct Stage {
		std::string in_class;
		std::string out_class;
		virtual void async(ParcelPtr) = 0;
	private:
		int in_classid;
		int out_classid;
	};

	int classify_parcel(const std::string&);
	void set_parcel_translator(int classid, ParcelTranslator*);
	ParcelTranslator* get_parcel_translator(int classid);
	void add_parcel_handler(int classid, ParcelHandler*);
	void push_json(std::istream&); // Note: do NOT use string or const, properity_tree only reads from non-const base_istream
	void push_parcel(ParcelPtr);
};

#endif
