#ifndef PREF_H
#define PREF_H

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <QtCore/QLibrary> 
#include <QtCore/QFileInfo> 
#include <caf/all.hpp>
#include "json.h"

using boost::property_tree::ptree;
using std::string;

class Pref {
public:
	static Pref* instance();

	Pref();
	void load_preference(int argc, char* argv[]);
	void scan_modules();
	void load_modules();
	void load_specific_module(const std::string&);
	void terminate_modules();

	string get_log_fn() const { return fn_log_; }
	FILE* get_flog() { return flog_; }
	string get_libpath() const;
	string get_webroot() const;

	void install_actor(const string& path, caf::actor actor);
	caf::actor match_actor(const string& path);
	caf::actor uninstall_actor(const string& path);

	shared_ptree get_registry();
	std::string get_pref(const std::string&) const;
	void set_registry(shared_ptree);
private:
	void load_single_module(QLibrary&);


	string fn_log_;
	string profile_;
	string module_path_;
	FILE* flog_;

	void load_defaults();
	std::vector<QFileInfo> libinfo_;
	std::vector<std::unique_ptr<QLibrary>> libs_;
	std::unordered_map<string, caf::actor> actor_table_;
	shared_ptree reg_;
};

#endif
