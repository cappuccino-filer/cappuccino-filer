#ifndef DEMO_HTTPREQ_H
#define DEMO_HTTPREQ_H

/*
 * Define C++ struct, which should inherit pipeline::Parcel
 */
struct HttpReq : public pipeline::Parcel {
	std::string url;
};

#endif
