#include <QDebug> 
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>

#include <server_http.hpp>
#include <pref.h>

#ifndef PORTAL_HTTP_UTILS_H
#define PORTAL_HTTP_UTILS_H

using namespace std;

namespace portal {

	typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

    // transform get request path into canonicalized path, 
    // i.e. remove all "..", "." characters
    // This method will throw 400 error if trying to leave webroot
    string canonicalize_get_url(shared_ptr<HttpServer::Request> req);

    // fill in fields for http response header
    void make_response_header(HttpServer::Response& response, string status, map<string, string> &header_info);

    // render static page with header
    void render(HttpServer::Response& response, string filename);

    void render_bad_request(HttpServer::Response& response);

    void render_not_found(HttpServer::Response& response);

    void render_internal_server_error(HttpServer::Response& response);

};

#endif /* end of include guard: PORTAL_HTTP_UTILS_H */
