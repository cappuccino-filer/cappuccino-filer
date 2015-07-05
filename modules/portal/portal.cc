#include <QDebug> 
#include <server_http.hpp>
#include <fstream>
#include <pref.h>
#include <util.h>
#include <pipeline.h>
#include <sstream>
#include "http_utils.h"

using namespace std;

namespace {
	typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
	HttpServer* server = nullptr;
	thread* server_thread;

	void http_file_fetch(HttpServer::Response& response, shared_ptr<HttpServer::Request> request)
	{
		//string filename("../webroot/");
		// string webroot(pref::instance()->get_webroot());
		string path=request->path;
		qDebug() << "HTTP request: " << path;

        try {
            path = portal::canonicalize_get_url(request);
        }
        catch (int n) {
            portal::render_bad_request(response);
            return;
        }

        portal::render(response, path);
	}
}

int init_httpd() 
{
	server = new HttpServer(8080, 1);
	server->default_resource["GET"] = http_file_fetch;
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
