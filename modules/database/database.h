#ifndef DATABASE_DATABASE_H
#define DATABASE_DATABASE_H

#include <memory>
#include "filestat.h"

/*
 * TODO: database interface
 *
 * We'll keep different kinds of information in our DB, so the interface
 * should be as general as possible.
 *
 * Maybe this class should be moved to core
 */

class Pref;

class Database {
public:
	Database(Pref*);
	virtual ~Database();

private:
};

typedef std::shared_ptr<Database> DatabasePtr;

#endif
