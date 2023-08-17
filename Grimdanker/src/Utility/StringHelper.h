#pragma once
#include <string>
class StringHelper
{
public:
	static const std::wstring StringToW(std::string str);
	static std::string GetExtension(std::string filename);
	static std::string GetName(std::string path);
	static std::string extractNumberStr(std::string str);
	static bool IsNumber(std::string path);
	static bool IsNumber(char c);
};