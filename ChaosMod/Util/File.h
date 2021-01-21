#pragma once

#include <sstream>

static bool DoesFileExist(const char* const fileName)
{
	struct stat temp;
	return stat(fileName, &temp) != -1;
}