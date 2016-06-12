#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <QtCore/QString>
#include <QDebug>

inline QString cvstr(const std::string& i) { return QString::fromUtf8(i.c_str()); }
inline QDebug operator << (QDebug& qdebug, const std::string& str) { return qdebug << cvstr(str); }

std::string sys_get_error_string();
#if 0
std::ostream& operator<<(std::ostream& fout, decltype(nullptr))
{
	fout << "<null>";
	return fout;
}
#endif

#endif
