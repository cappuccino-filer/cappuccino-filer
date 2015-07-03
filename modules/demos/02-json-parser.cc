/*
 * Demo 02: json parser
 *
 * Our backend accepts and delivers json objects as input/output from/to UI.
 * However, for easy access, we want C++ structs instead of text strings in
 * our processing procedure. Therefore we need to translate json to C++ structs
 *
 */
#include <pipeline.h>
#include <util.h>
#include "httpreq.h" // Define the corresponding C++ struct

using pipeline::ParcelPtr;

namespace {
namespace pt = boost::property_tree;

struct HttpReqTr : public pipeline::ParcelTranslator {
	virtual ParcelPtr parse_json(const pt::ptree& tree)
	{
		try {
			auto ret = std::shared_ptr<HttpReq>(new HttpReq);
			ret->url = tree.get<std::string>("url");
			// Note: we don't need to set classid inherited from
			// Parcel. It will be set up by the caller (push_json).
			return ret;
		} catch (std::runtime_error& err) {
			// Output errors here, the caller won't do it.
			qDebug() << "Runtime error: " << err.what();
		}
		return ParcelPtr(nullptr);
	}
};

HttpReqTr prototype;
}

extern "C" {

int draft_module_init()
{
	// Register translator
	pipeline::set_parcel_translator(pipeline::classify_parcel("http_get_url"), &prototype);
	return 0;
}

int draft_module_term()
{
	return 0;
}

};
