/*
 * Demo 0: Basic module
 *
 * The module loader will check every shared library under specified path(s),
 * and then check the existence of entrance functions: cappuccino_filer_module_init and
 * cappuccino_filer_module_term
 *
 * After that the module loader will call cappuccino_filer_module_init. A successful
 * loaded module should return a non-negative integer.
 *
 * On program exit, the loader will call cappuccino_filer_module_term.
 *
 */
extern "C" {

int cappuccino_filer_module_init()
{
	return 0;
}

int cappuccino_filer_module_term()
{
	return 0;
}

};
