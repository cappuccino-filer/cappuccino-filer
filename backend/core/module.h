#ifndef MODULE_H
#define MODULE_H

class pref;

typedef int (*draft_module_init)();
typedef int (*draft_module_term)();
#define MODULE_INIT_NAME "draft_module_init"
#define MODULE_TERM_NAME "draft_module_term"

#endif
