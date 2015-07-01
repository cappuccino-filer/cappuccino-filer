#include <getopt.h>
#include <QDebug> 
#include <QtCore/QDirIterator> 
#include "pref.h"
#include "util.h"
#include "module.h"

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
		qDebug() << "Open "
			<<  fn_log_.c_str()
			<< " for logging, all upcoming messages will be redirected";
		flog_ = file;
	}
}

void pref::load_modules()
{
	QDirIterator di(cvstr(get_libpath()));
	while (di.hasNext()) {
		di.next();
		auto fi = di.fileInfo();
		auto fn = fi.filePath();
		qDebug() << "Checking " << fn;
		if (!fi.isFile())
			continue;
		if (!fi.exists())
			continue;
		if (!QLibrary::isLibrary(fn))
			continue;
		auto lib = new QLibrary(fn);
		libs_.emplace_back(lib);
	} 

	for (auto& plib : libs_) {
		auto& lib = *plib;
		auto fn = lib.fileName();
		int ret = -1;
		auto init = (draft_module_init) (lib.resolve(MODULE_INIT_NAME));
		auto term = (draft_module_term) (lib.resolve(MODULE_TERM_NAME));
		if (init && term) {
			ret = (*init)();
		} else {
			qDebug() << "Library " << fn
				<< " does not have function "
				<< MODULE_INIT_NAME
				<< " or "
				<< MODULE_TERM_NAME;
		}
		if (ret < 0) {
			qDebug() << "Unload " << fn << " since it failed to initialize";
			lib.unload();
		} else {
			qDebug() << "Successfully loaded " << fn ;
		}
	}
}

void pref::terminate_modules()
{
	for (auto& plib : libs_) {
		auto& lib = *plib;
		auto fn = lib.fileName();
		if (!lib.isLoaded())
			continue;
		qDebug() << "Stopping module " << fn;
		auto term = (draft_module_term) (lib.resolve(MODULE_TERM_NAME));
		int ret = (*term)();
		if (ret < 0)
			qWarning() << "Module " << fn
				<< " failed to terminate.";
		else
			qDebug() << "Functions from Module " << fn
				<< " terminated.";
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

std::string pref::get_libpath() const
{
	return std::string(".");
}

std::string pref::get_webroot() const
{
	return std::string("../webroot");
}
