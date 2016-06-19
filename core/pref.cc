#include <getopt.h>
#include <QDebug> 
#include <QtCore/QDirIterator> 
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "pref.h"
#include "util.h"
#include "module.h"

enum {
	OPT_LOG_FILE = 1,
	OPT_PROFILE,
};

static struct option opts[] = {
	{"log",		required_argument,	0,	OPT_LOG_FILE},
	{"profile",	required_argument,	0,	OPT_PROFILE},
	{NULL,		no_argument,		NULL,		0}
};

using boost::property_tree::json_parser::read_json;

void Pref::load_preference(int argc, char* argv[])
{
	int longidx;
	int val;
	do {
		val = getopt_long_only(argc, argv, "", opts, NULL);
		switch (val) {
			case OPT_LOG_FILE:
				fn_log_ = std::string(optarg);
				break;
			case OPT_PROFILE:
				profile_ = std::string(optarg);
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
	
	if (!profile_.empty()) {
		std::ifstream fin(profile_);
		if (!fin.is_open()) {
			qDebug() << "Failed to open profile: " << profile_.c_str();
		} else {
			read_json(fin, *reg_);
		}
	}
}

void Pref::scan_modules()
{
	qDebug() << "Scan " << get_libpath() << " for modules ";
	QDir dir(cvstr(get_libpath()));
	auto filist = dir.entryInfoList(QDir::Files|QDir::Executable, QDir::Name);
	for(auto fi : filist) {
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
		libinfo_.emplace_back(fi);
	} 
}

void Pref::load_modules()
{
	scan_modules();
	for (auto& plib : libs_)
		load_single_module(*plib);
}

void Pref::load_specific_module(const std::string& name)
{
	auto match = QString::fromStdString(name);
	for(size_t i = 0; i < libs_.size(); i++) {
		if (libinfo_[i].baseName() == match) {
			load_single_module(*libs_[i]);
		}
	}
}

void Pref::load_single_module(QLibrary& lib)
{
	auto fn = lib.fileName();
	int ret = -1;
	auto init = (cappuccino_filer_module_init) (lib.resolve(MODULE_INIT_NAME));
	auto term = (cappuccino_filer_module_term) (lib.resolve(MODULE_TERM_NAME));
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

void Pref::terminate_modules()
{
	for (auto& plib : libs_) {
		auto& lib = *plib;
		auto fn = lib.fileName();
		if (!lib.isLoaded())
			continue;
		qDebug() << "Stopping module " << fn;
		auto term = (cappuccino_filer_module_term) (lib.resolve(MODULE_TERM_NAME));
		int ret = (*term)();
		if (ret < 0)
			qWarning() << "Module " << fn
				<< " failed to terminate.";
		else
			qDebug() << "Functions from Module " << fn
				<< " terminated.";
	}
}

Pref::Pref()
{
	reg_ = std::make_shared<ptree>();
	load_defaults();
}

void Pref::load_defaults()
{
	module_path_ = ".";
	flog_ = stderr;
	reg_->put("core.toolpath", "tools/");
	reg_->put("core.database", "mariadb");
	reg_->put("core.libpath", "./modules");
	reg_->put("core.libexecpath", "./");
	reg_->put("mariadb.host", "localhost");
	reg_->put("mariadb.user", "test");
	reg_->put("mariadb.password", "test");
	reg_->put("mariadb.database", "draft");
	reg_->put("mariadb.port", 0);
	reg_->put("mariadb.unix_socket", "");
	reg_->put("mariadb.client_flag", 0);
	// CAVEAT: REMOVE THIS IF RELEASED
	reg_->put("mariadb.debug", true);
	reg_->put("portal.webroot", "../webroot");
}

Pref* Pref::instance()
{
	static Pref inst_;
	return &inst_;
}

std::string Pref::get_libpath() const
{
	return get_pref("core.libpath");
}

std::string Pref::get_webroot() const
{
	return get_pref("portal.webroot");
}

void Pref::install_actor(const std::string& path, caf::actor actor)
{
	actor_table_[path] = actor;
}

caf::actor Pref::match_actor(const std::string& path)
{
	auto f = actor_table_.find(path);
	if (f == actor_table_.end())
		throw 403;
	return f->second;
}

caf::actor Pref::uninstall_actor(const std::string& path)
{
	auto f = actor_table_.find(path);
	if (f != actor_table_.end()) {
		auto ret = f->second;
		actor_table_.erase(f);
		return ret;
	}
	return caf::invalid_actor;
}

using std::string;

shared_ptree Pref::get_registry()
{
	return reg_;
}

std::string Pref::get_pref(const std::string& path) const
{
	return reg_->get(path, "");
}

void Pref::set_registry(shared_ptree newpt)
{
	reg_.swap(newpt);
}
