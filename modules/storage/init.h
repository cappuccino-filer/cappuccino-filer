#ifndef STORAGE_INIT_H
#define STORAGE_INIT_H

/*
 * CAVEAT: only for applications linked to this library directly.
 *
 * e.g. tool/updatedb
 */
int storage_module_init();
int storage_module_term();

#endif
