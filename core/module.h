#ifndef MODULE_H
#define MODULE_H

class pref;

typedef int (*cappuccino_filer_module_init)();
typedef int (*cappuccino_filer_module_term)();
#define MODULE_INIT_NAME "cappuccino_filer_module_init"
#define MODULE_TERM_NAME "cappuccino_filer_module_term"

#endif
