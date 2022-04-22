#pragma once

#include <sstream>

inline bool DoesFileExist(const char* szFileName)
{
	struct stat temp;
	return stat(szFileName, &temp) != -1;
}