#include "change_character_set.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
string ChangeCharacterSet::GbkToUtf8(const char* src_str)
{
	int len = MultiByteToWideChar(0, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(0, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(65001, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(65001, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

string ChangeCharacterSet::Utf8ToGbk(const char* src_str)
{
	int len = MultiByteToWideChar(65001, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(65001, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(0, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(0, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
#endif