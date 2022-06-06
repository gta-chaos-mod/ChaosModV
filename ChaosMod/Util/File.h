#pragma once

#include <sstream>
#include <sys/stat.h>

inline bool DoesFileExist(const char *szFileName)
{
	struct stat temp;
	return stat(szFileName, &temp) == 0;
}