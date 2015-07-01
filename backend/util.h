#ifndef UTIL_H
#define UTIL_H

#include <QtCore/QString>
#include <string>

inline QString cvstr(const std::string& i) { return QString::fromUtf8(i.c_str()); }

#endif
