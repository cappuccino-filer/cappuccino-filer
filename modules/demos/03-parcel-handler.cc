/*
 * Demo 03: parcel handler
 *
 * Now we have C++ structs (Parcel) translated from json objects. Now we are
 * going to implement handlers to process structs.
 *
 */
#include <boost/thread/future.hpp> // boost::async and future
#include <pipeline.h>
#include <util.h>
#include "httpreq.h" // Define the corresponding C++ struct

using pipeline::ParcelPtr;

namespace {
namespace pt = boost::property_tree;

struct HttpReqHandler : public pipeline::ParcelHandler {
	virtual void async_handle(ParcelPtr parcel)
	{
		// Downcast to its actual type
		auto req = std::static_pointer_cast<HttpReq>(parcel);
		// Use boost::async because of boost::future::then
		// We shall demonstrate future::then in more complex cases.
		auto fut = boost::async([req](){
				qDebug() << "URL: " << req->url;
				});
	}
};

HttpReqHandler prototype;
}

extern "C" {

int draft_module_init()
{
	// Register handler
	pipeline::add_parcel_handler(pipeline::classify_parcel("http_get_url"), &prototype);
	return 0;
}

int draft_module_term()
{
	return 0;
}

};
