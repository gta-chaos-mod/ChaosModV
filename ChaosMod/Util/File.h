#pragma once

#include <sys/stat.h>

#include <sstream>
#include <string_view>

inline bool DoesFileExist(std::string_view fileName)
{
	struct stat temp;
	return stat(fileName.data(), &temp) == 0;
}