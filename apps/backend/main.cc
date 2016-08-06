#include <pref.h>
#include <io_engine.h>
#include <logger.h>
#include <caf/all.hpp>
#include <stdexcept>
#include <QDebug>

int main(int argc, char* argv[])
{
	try {
		Pref::instance()->load_preference(argc, argv);
		logger::init();
		Pref::instance()->load_modules();
		io_engine::init();
	} catch (std::exception& e) {
		qDebug() << e.what();
		logger::dump();
		return 1;
	}
	io_engine::run();
	Pref::instance()->terminate_modules();
	Pref::instance()->save_preference();
	logger::dump();

	// Cleanup CAF
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}
