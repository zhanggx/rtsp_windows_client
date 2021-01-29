#pragma once
#include <string>

class _declspec(dllexport) Utils
{
public:
	static std::wstring getAppDataRoamingPathW();
	static std::string getAppDataRoamingPath();
};

