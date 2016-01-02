#ifndef PREF_H
#define PREF_H

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <QtCore/QLibrary> 
#include <caf/all.hpp>

class Pref {
public:
	static Pref* instance();

	Pref();
	void load_preference(int argc, char* argv[]);
	void load_modules();
	void terminate_modules();

	std::string get_log_fn() const { return fn_log_; }
	FILE* get_flog() { return flog_; }
	std::string get_libpath() const;
	std::string get_webroot() const;

	void install_actor(const std::string& path, caf::actor actor);
	caf::actor match_actor(const std::string& path);
	caf::actor uninstall_actor(const std::string& path);
private:
	std::string fn_log_;
	std::string module_path_;
	FILE* flog_;

	void load_defaults();
	std::vector<std::unique_ptr<QLibrary>> libs_;
	std::unordered_map<std::string, caf::actor> actor_table_;
};

#endif
