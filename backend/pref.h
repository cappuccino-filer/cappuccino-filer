#ifndef PREF_H
#define PREF_H

#include <string>
#include <stdio.h>

class pref {
public:
	static pref* instance();

	pref();
	void load_preference(int argc, char* argv[]);
	std::string get_log_fn() const { return fn_log_; }
	FILE* get_flog() { return flog_; }
private:
	std::string fn_log_;
	std::string module_path_;
	FILE* flog_;

	void load_defaults();
};

#endif
