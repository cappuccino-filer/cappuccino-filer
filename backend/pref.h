#ifndef PREF_H
#define PREF_H

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <QtCore/QLibrary> 

class pref {
public:
	static pref* instance();

	pref();
	void load_preference(int argc, char* argv[]);
	void load_modules();
	void terminate_modules();

	std::string get_log_fn() const { return fn_log_; }
	FILE* get_flog() { return flog_; }
	std::string get_libpath() const;
private:
	std::string fn_log_;
	std::string module_path_;
	FILE* flog_;

	void load_defaults();
	std::vector<std::unique_ptr<QLibrary>> libs_;
};

#endif
