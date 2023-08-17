#include "StringHelper.h"

const std::wstring StringHelper::StringToW(std::string str)
{
	return std::wstring(str.begin(), str.end());
}

std::string StringHelper::GetExtension(std::string filename)
{
	size_t offset = filename.find_last_of('.');
	return std::string(filename.substr(offset + 1));
}

std::string StringHelper::GetName(std::string path)
{
	size_t offset = path.find_last_of("/");
	size_t offset2 = path.find_last_of("\\");
	return std::string(path.substr(offset + offset2 + 2));
}

std::string StringHelper::extractNumberStr(std::string str)
{
	size_t offset = 0;
	for (int i = 0; i < str.size(); i++)
	{
		std::string converter;
		converter = str[i];
		if (IsNumber(converter))
		{
			offset = i;
			break;
		}
	}
	size_t offset2 = 0;
	for (int i = str.size()-1; i > 0; i--)
	{
		std::string converter;
		converter = str[i];
		if (IsNumber(converter))
		{
			offset2 = i;
			break;
		}
	}
	std::string number = str.substr(offset, (offset2+1) - offset);

	return number;
}

bool StringHelper::IsNumber(std::string s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c) || c == '.'; }) == s.end();
}

bool StringHelper::IsNumber(char c)
{
	return std::isdigit(c);
}
