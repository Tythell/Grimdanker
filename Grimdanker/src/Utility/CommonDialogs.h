#pragma once
#include <string>
#include <Windows.h>
#include <commdlg.h>
#include <vector>
//#include <shobjidl_core.h>

class Dialogs
{
public:
	/// <summary>
	/// Works without setting owner handle just fine. Use this if you want to bind the dialogs to a window
	/// </summary>
	/// <param name="h"></param>
	static void SetOwnerHandle(HWND* h);

	/// <summary>
	/// Returns a string of file opened
	/// </summary>
	/// <param name="filter: Written like; (Emample .exmpl)\0*.exmpl;"></param>
	/// <param name="subDir: Initial directory"></param>
	/// <param name="nroffilters"></param>
	/// <returns></returns>
	static std::string OpenFile(const char* filter, const char* subDir = "", int nroffilters = 1);

	static std::vector<std::string> CharsToStringsVector(const char* fullstring, const int& sizeofString, const char& separtor);

	/// <summary>
	/// Returns a vector of strings where string [0] is the the selected directory and the rest are selected file names
	/// </summary>
	/// <param name="filter: Written like; (Emample .exmpl)\0*.exmpl;"></param>
	/// <param name="subDir: Initial directory"></param>
	/// <param name="nroffilters"></param>
	/// <returns></returns>
	static std::vector<std::string> OpenMultifile(const char* filter, const char* subDir = "", int nroffilters = 1);

	/// <summary>
	/// Returns the path chosen from the SaveFile Window as a string
	/// </summary>
	/// <param name="filter"></param>
	/// <param name="subDir"></param>
	/// <returns></returns>
	static std::string SaveFile(const char* filter, const char* subDir = "");
private:
	static HWND* m_ownerhandle;
};