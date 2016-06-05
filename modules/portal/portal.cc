#include <QDebug> 
#include <fstream>
#include <pref.h>
#include <json.h>
#include <util.h>
#include <sstream>
#include "server_http.hpp"

#include "http_utils.h"
#include <caf/all.hpp>

using namespace std;

namespace {
	typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
	HttpServer* server = nullptr;
	thread* server_thread;
	caf::actor resp_actor;

	boost::promise<int> mkpromise(int value) { boost::promise<int> ret; ret.set_value(value); return ret; }

	boost::future<int> http_file_fetch(HttpServer::ResponsePtr response, shared_ptr<HttpServer::Request> request)
	{
		string path = request->path;
		qDebug() << "HTTP GET request: " << path;

		try {
			path = portal::canonicalize_get_url(request);
		} catch (int n) {
			portal::render_bad_request(*response);
			return mkpromise(0).get_future();
		}

		portal::render(*response, path);
		return mkpromise(0).get_future();
	}

	using boost::property_tree::write_json;

	caf::behavior reply_json(
			caf::event_based_actor* self,
			shared_ptr<HttpServer::Request> request,
			HttpServer::ResponsePtr response
			)
	{
		return { [=] (const shared_ptree reply) 
			{
				stringstream jsonstream;
				write_json(jsonstream, *reply, false);
				string jsonstr = jsonstream.str();

				string status = "200 OK";
				map<string, string> header_info = {
					{ "Content-Length", to_string(jsonstr.size()) }
				};

				portal::make_response_header(*response, status, header_info);
				(*response) << jsonstr;
				std::cout << jsonstr;

				response->promise.set_value(0);
				self->quit();
			}
		};
	}

	boost::future<int> http_api(HttpServer::ResponsePtr response, shared_ptr<HttpServer::Request> request)
	{
		qDebug() << request->method.c_str() << " on " << request->path.c_str();
		auto pt = std::make_shared<boost::property_tree::ptree>();
		try {
			boost::property_tree::read_json(request->content, *pt);
		} catch (...) {
		}
		pt->put("method", request->method);
		try {
			auto path = portal::canonicalize_get_url(request);
			auto handler = Pref::instance()->match_actor(request->path);
			response->promise = boost::promise<int>();
			auto fut = response->promise.get_future();
			auto rep_actor = caf::spawn(reply_json, request, response);
			qDebug() << "Deliver request to actor " << &rep_actor;
			send_as(rep_actor, handler, pt);
			return fut;
		} catch (int n) {
			portal::render_bad_request(*response);
			return mkpromise(0).get_future();
		}
	}
}

int init_httpd() 
{
	server = new HttpServer(8080, 1);
	server->default_resource["GET"]  = http_file_fetch;
	server->resource["^/api/.*"]["POST"] = http_api;
	server->resource["^/api/.*"]["GET"] = http_api;
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

int cappuccino_filer_module_init()
{
	int ret;
	try {
		ret = init_httpd();
	} catch (...) {
		return -1;
	}
	return ret;
}

int cappuccino_filer_module_term()
{
	qDebug() << "Stopping HTTP server and worker thread";
	server->stop();
	qDebug() << "HTTP server stopped";
	server_thread->join();
	qDebug() << "HTTP worker thread stopped";
	return 0;
}

};
