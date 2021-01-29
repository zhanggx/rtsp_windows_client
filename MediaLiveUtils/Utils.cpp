#include "Utils.h"

#include <iostream>
#include <windows.h>
#include <processenv.h>

std::wstring Utils::getAppDataRoamingPathW()
{
    wchar_t roaming[MAX_PATH] = { 0 };
    DWORD size = GetEnvironmentVariable(L"appdata", roaming, MAX_PATH);
    if (size >= MAX_PATH || size == 0) {
	    std::cout << "fail to get %appdata%: " << GetLastError() << std::endl;
        return L"";
    }
    else {
	    std::wstring result(roaming, size);
        result += L"\\MediaLiveClient";
        return result;// string2MMKVPath_t(result);
    }
}
std::string Utils::getAppDataRoamingPath()
{
    char roaming[MAX_PATH] = { 0 };
    DWORD size = GetEnvironmentVariableA("appdata", roaming, MAX_PATH);
    if (size >= MAX_PATH || size == 0) {
        std::cout << "fail to get %appdata%: " << GetLastError() << std::endl;
        return "";
    }

    std::string result(roaming, size);
    result += "\\MediaLiveClient";
    return result;// string2MMKVPath_t(result);
}