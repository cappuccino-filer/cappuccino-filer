#ifndef UTIL_H
#define UTIL_H

#include <QtCore/QString>
#include <QDebug>
#include <string>

inline QString cvstr(const std::string& i) { return QString::fromUtf8(i.c_str()); }
inline QDebug operator << (QDebug& qdebug, const std::string& str) { return qdebug << cvstr(str); }

#endif
