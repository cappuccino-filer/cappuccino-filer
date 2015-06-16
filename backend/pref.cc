#include <getopt.h>
#include <QDebug> 
#include "pref.h"

enum {
	OPT_LOG_FILE = 1,
};

static struct option opts[] = {
	{"log",		required_argument,	0,	OPT_LOG_FILE},
	{NULL,		no_argument,		NULL,		0}
};

void pref::load_preference(int argc, char* argv[])
{
	int longidx;
	int val;
	do {
		val = getopt_long_only(argc, argv, "", opts, NULL);
		switch (val) {
			case OPT_LOG_FILE:
				fn_log_ = std::string(optarg);
				break;
			default:
				break;
		}
	} while (val > 0);
	if (!fn_log_.empty()) {
		FILE* file = fopen(fn_log_.c_str(), "a+");
		if (!file) {
			qDebug() <<"Cannot open "<< fn_log_.c_str();
			throw fn_log_;
		}
		qDebug() << "Open " <<  fn_log_.c_str() << " for logging, all upcoming messages will be redirected";
		flog_ = file;
	}
}

pref::pref()
{
	load_defaults();
}

void pref::load_defaults()
{
	module_path_ = ".";
	flog_ = stderr;
}

pref* pref::instance()
{
	static pref inst_;
	return &inst_;
}
