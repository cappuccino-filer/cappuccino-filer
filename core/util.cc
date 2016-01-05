#include "util.h"
#include <string.h>
#include <errno.h>

std::string sys_get_error_string()
{
	char buf[1024];
	buf[0] = 0;
	strerror_r(errno, buf, 1024);
	return std::string(buf);
}
