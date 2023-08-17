#pragma once
#include "COMException.h"
class Popup
{
public:
	static void Error(std::string err = "404 not found");
	static void Info(std::string err = "");
	static void Error(COMException& exception);
};