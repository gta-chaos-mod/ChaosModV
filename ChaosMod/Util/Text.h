#pragma once

inline std::string StringTrim(std::string str)
{
	if (str.find_first_not_of(' ') == str.npos)
	{
		return "";
	}

	str = str.substr(str.find_first_not_of(' '));
	str = str.substr(0, str.find_first_of('\0'));
	str = str.substr(0, str.find_last_not_of(' ') == str.npos ? str.npos : str.find_last_not_of(' ') + 1);
	return str;
};