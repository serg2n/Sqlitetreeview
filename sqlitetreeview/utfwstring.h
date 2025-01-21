#ifndef UTFWSTRING_H
#define UTFWSTRING_H

#include <string>
#include <codecvt>
#include <vector>
#include<sstream>
#include <iostream>
#include <fstream>
#include <iomanip>


std::wstring utf8_to_wstring(std::string const& str);
std::string wstring_to_utf8(std::wstring const& str);
std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter);
std::string ucharToHexString(unsigned char c);
std::wstring ucharToHexWstring(unsigned char c);
std::wstring VectorToWString(const std::vector<unsigned char>& vec);
void WriteVectorToFile(const std::vector<unsigned char>& vec, const std::string& filename);
void WriteVectorToTextFile(const std::vector<unsigned char>& vec, const std::string& filename);
#endif // UTFWSTRING_H
