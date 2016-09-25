#include "http_utils.h"

#include <QDebug> 
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>

#include <pref.h>

using namespace std;

namespace portal {

	// Auxiliary methods
	void tokenize(const string& url, const char* delimiter, vector<string> &tokens)
	{
		assert(url.length() > 0);
		assert(delimiter);

		boost::char_separator<char> sep(delimiter);
		boost::tokenizer<boost::char_separator<char> > tok(url, sep);
		for(auto it = tok.begin(); it != tok.end(); ++it){
			tokens.push_back(string(*it));
		}
	}

	// API

	void make_response_header(HttpServer::Response& response, string status, map<string, string> &header_info) 
	{
		response << "HTTP/1.1 " << status << "\r\n";
		for (auto& kvpair : header_info) {
			response << kvpair.first << ": " << kvpair.second << "\r\n";
		}
		response << "\r\n";
	}

	string canonicalize_get_url(shared_ptr<HttpServer::Request> req)
	{
		assert(req);

		string url = req->path;
		vector<string> tokens;
		vector<string> canonicalized;
		tokenize(url, "/", tokens);

		// process tokens
		for (auto token : tokens) {
#if 0
			std::cout << "Token: " << token << std::endl;
#endif
			if (token == string("..")) {
				if (canonicalized.size() == 0) throw 400; // bad request
				canonicalized.pop_back();
			}
			else if (token == string(".")) {
				// do nothing
			}
			else {
				canonicalized.push_back(token);
			}
		}

		string ret = boost::algorithm::join(canonicalized, "/");
		return string("/") + ret;
	}

	bool endswith(const string& haystack, const string& suffix)
	{
		if (haystack.length() >= suffix.length()) {
			return (0 == haystack.compare(haystack.length() - suffix.length(), suffix.length(), suffix));
		} else {
			return false;
		}
	}

	const map<string, string> suffix_mime = {
		{ ".html", "text/html" },
		{ ".js", "application/javascript" },
		{ ".css", "text/css" },
	};

	string guess_type_from_name(const string& fn)
	{
		for (const auto& pair : suffix_mime) {
			if (endswith(fn, pair.first)) {
				return pair.second;
			}
		}
		return string();
	}

	void render(HttpServer::Response& response, const string& reqpath) 
	{
		string webroot(Pref::instance()->get_webroot());
		string filename = webroot + reqpath;

		if (boost::filesystem::is_directory(filename)) 
			filename += string("/index.html");

		ifstream ifs;
		ifs.open(filename, ifstream::in);

		bool notFound = false;
		// consider to make it more flexible, one should retrieve 404 page url 
		// by configuration
		if (!ifs.good()) {
			notFound = true;
			ifs.open(webroot + string("/assets/404.html"), ifstream::in);
		}
		assert(ifs.good());

		ifs.seekg(0, ios::end);
		size_t length = ifs.tellg();

		ifs.seekg(0, ios::beg);

		map<string, string> header_info = {
			//{ "class",          "http_file_fetch" },
			{ "Content-Length", to_string(length) },
		};
		auto content_type = guess_type_from_name(filename);
		if (!content_type.empty())
			header_info["Content-Type"] = content_type;
		string status = notFound ? "404 NOT FOUND" : "200 OK";
		make_response_header(response, status, header_info);

		// read and send 128 KB at a time if file-size>buffer_size
		size_t buffer_size=131072;
		if (length > buffer_size) {
			vector<char> buffer(buffer_size);
			size_t read_length;
			while ((read_length=ifs.read(&buffer[0], buffer_size).gcount()) > 0) {
				response.stream.write(&buffer[0], read_length);
				response << HttpServer::flush;
			}
		} else
			response << ifs.rdbuf();

		ifs.close();
	}

	void render_bad_request(HttpServer::Response& response)
	{
		render(response, "/assets/400.html");
	}

	void render_not_found(HttpServer::Response& response)
	{
		render(response, "/assets/404.html");
	}

	void render_internal_server_error(HttpServer::Response& response)
	{
		render(response, "/assert/500.html");
	}

};
