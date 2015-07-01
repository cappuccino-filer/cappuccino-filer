#include "pref.h"
#include "io_engine.h"
#include "logger.h"

int main(int argc, char* argv[])
{
	try {
		pref::instance()->load_preference(argc, argv);
		logger::init();
		pref::instance()->load_modules();
		io_engine::init();
	} catch (...) {
		logger::dump();
		return 1;
	}
	io_engine::run();
	pref::instance()->terminate_modules();
	logger::dump();
	return 0;
}
