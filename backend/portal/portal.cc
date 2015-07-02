#include <QDebug> 
#include <server_http.hpp>
#include <fstream>
#include <pref.h>
#include <util.h>

using namespace std;

namespace {
	typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
	HttpServer* server = nullptr;
	thread* server_thread;

	void http_file_fetch(HttpServer::Response& response, shared_ptr<HttpServer::Request> request)
	{
		//string filename("../webroot/");
		string filename(pref::instance()->get_webroot());
		string path=request->path;
		qDebug() << "HTTP request: " << path;

		// Replace all ".." with "." (so we can't leave the web-directory)
		size_t pos;
		while ((pos=path.find(".."))!=string::npos) {
			path.erase(pos, 1);
		}
		if (path.find('.') == string::npos) {
			if (path.back() != '/')
				path += '/';
			path += "index.html";
		}

		filename+=path;
		ifstream ifs;
		// A simple platform-independent file-or-directory check does not exist, but this works in most of the cases:
		qDebug() << "Trying to open file " << filename;
		ifs.open(filename, ifstream::in);

		if (ifs) {
			ifs.seekg(0, ios::end);
			size_t length=ifs.tellg();

			ifs.seekg(0, ios::beg);

			response << "HTTP/1.1 200 OK\r\nContent-Length: "
				<< length
				<< "\r\n\r\n";
			// read and send 128 KB at a time if file-size>buffer_size
			size_t buffer_size=131072;
			if (length > buffer_size) {
				vector<char> buffer(buffer_size);
				size_t read_length;
				while ((read_length=ifs.read(&buffer[0], buffer_size).gcount()) > 0) {
					response.stream.write(&buffer[0], read_length);
					response << HttpServer::flush;
				}
			}
			else
				response << ifs.rdbuf();

			ifs.close();
		} else {
			string content="Could not open file "+filename;
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
		}
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
