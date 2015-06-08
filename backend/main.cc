#include "pref.h"
#include "io_engine.h"
#include "logger.h"
#include "database/database.h"
#include "storage/storage.h"
#include "portal/portal.h"
#include "tag/tag.h"

int main(int argc, char* argv[])
{
	pref::load_preference(argc, argv);
	logger::launch();
	try {
		io_engine::launch();
		database::launch();
		storage::launch();
		tag::launch();
		portal::launch();
	} catch (...) {
		logger::dump();
		return 1;
	}
	io_engine::run();
	return 0;
}
