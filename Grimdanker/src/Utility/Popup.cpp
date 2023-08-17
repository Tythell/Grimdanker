#include "Popup.h"

void Popup::Error(std::string err)
{
	std::string str = "Error: " + err;
	MessageBoxA(NULL, str.c_str(), "Joe mama", MB_ICONERROR);
}

void Popup::Info(std::string err)
{
	std::string str = "Error: " + err;
	MessageBoxA(NULL, str.c_str(), "Joe mama", MB_ICONINFORMATION);
}

void Popup::Error(COMException& exception)
{
	std::wstring errMsg = exception.what();
	MessageBoxW(NULL, errMsg.c_str(), L"Error", MB_ICONERROR);
}

//void Popup::ComError(HRESULT hr, std::wstring msg)
//{
//	_com_error error(hr);
//	std::wstring msg = L"Error: " + msg + L"\n" + error.ErrorMessage();
//	MessageBoxW(NULL, msg.c_str(), L"Error", MB_ICONERROR);
//}
