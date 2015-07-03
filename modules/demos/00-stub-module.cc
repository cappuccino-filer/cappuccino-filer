/*
 * Demo 0: Basic module
 *
 * The module loader will check every shared library under specified path(s),
 * and then check the existence of entrance functions: draft_module_init and
 * draft_module_term
 *
 * After that the module loader will call draft_module_init. A successful
 * loaded module should return a non-negative integer.
 *
 * On program exit, the loader will call draft_module_term.
 *
 */
extern "C" {

int draft_module_init()
{
	return 0;
}

int draft_module_term()
{
	return 0;
}

};
