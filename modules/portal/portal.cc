#include <QDebug> 
#include <server_http.hpp>
#include <fstream>
#include <pref.h>
#include <util.h>
#include <pipeline.h>
#include <sstream>

//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "http_utils.h"

using namespace std;

namespace {
	typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
	HttpServer* server = nullptr;
	thread* server_thread;

	void http_file_fetch(HttpServer::Response& response, shared_ptr<HttpServer::Request> request)
	{
		string path = request->path;
		qDebug() << "HTTP GET request: " << path;

		try {
			path = portal::canonicalize_get_url(request);
		} catch (int n) {
			portal::render_bad_request(response);
			return;
		}

		portal::render(response, path);
	}

	void http_post_json(HttpServer::Response& response, shared_ptr<HttpServer::Request> request)
	{
#if 0
		stringstream ss;
		request->content >> ss.rdbuf();
		string content=ss.str();
		qDebug() << "HTTP POST request to " << request->path << " content: " << content;
		request->content.seekg(0, ios::beg);
#endif

		boost::property_tree::ptree pt;
		boost::property_tree::read_json(request->content, pt);

		string status = "200 OK";
		stringstream ss;
		ss << "The class is " << pt.get<string>("class") << "\n";
		ss << "Welcome! You have found the secret POST method!\n";
		string cont = ss.str();
		map<string, string> header_info = {
			{ "Content-Length", to_string(cont.size()) }
		};
		portal::make_response_header(response, status, header_info);
		response << cont;

		// temporary implementation
		//response << HttpServer::flush;
		qDebug() << "HTTP POST response: " << request->path;
	}
}

int init_httpd() 
{
	server = new HttpServer(8080, 1);
	server->default_resource["GET"]  = http_file_fetch;
	server->default_resource["POST"] = http_post_json;
	auto pserver = server;
	qDebug() << "Creating HTTP worker thread with server " << server;
	server_thread = new thread([pserver](){
			qDebug() << "Starting HTTP server with server " << pserver;
			pserver->start();
			qDebug() << "HTTP server started";
			});
	return 0;
}

extern "C" {

int draft_module_init()
{
	int ret;
	try {
		ret = init_httpd();
	} catch (...) {
		return -1;
	}
	return ret;
}

int draft_module_term()
{
	qDebug() << "Stopping HTTP server and worker thread";
	server->stop();
	qDebug() << "HTTP server stopped";
	server_thread->join();
	qDebug() << "HTTP worker thread stopped";
	return 0;
}

};
