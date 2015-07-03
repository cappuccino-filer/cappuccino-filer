/*
 * Demo 1: Module API
 *
 * All interfaces declared in core can be accessed by modules.
 * Besides this, Qt's debug function is also accessible.
 *
 */
#include <util.h> // QDebug and helper functions.
#include <pref.h> // Access preferences

extern "C" {

int draft_module_init()
{
	/*
 	 * On load this module will access pref and output libpath preference
	 * to debugging output.
	 */
	qDebug() << "[Pref] libpath: " << pref::instance()->get_libpath();
	return 0;
}

int draft_module_term()
{
	return 0;
}

};
