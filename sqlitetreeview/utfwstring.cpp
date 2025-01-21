
#include "utfwstring.h"

//------------------------------------------
std::wstring utf8_to_wstring(std::string const& str)
{
  std::wstring_convert<std::conditional_t<
        sizeof(wchar_t) == 4,
        std::codecvt_utf8<wchar_t>,
        std::codecvt_utf8_utf16<wchar_t>>> converter;
  return converter.from_bytes(str);
}
std::string wstring_to_utf8(std::wstring const& str)
{
  std::wstring_convert<std::conditional_t<
        sizeof(wchar_t) == 4,
        std::codecvt_utf8<wchar_t>,
        std::codecvt_utf8_utf16<wchar_t>>> converter;
  return converter.to_bytes(str);
}
std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter)
{
    std::vector<std::wstring> tokens;
    std::wstring token;
    std::wstringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
       tokens.push_back(token);
    }
    return tokens;
}
std::string ucharToHexString(unsigned char c)
{
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    return ss.str();
}
std::wstring ucharToHexWstring(unsigned char c)
{
    std::wstringstream ss;
    ss << std::hex << std::setw(2) << std::setfill(L'0') << static_cast<int>(c);
    return ss.str();
}
std::wstring VectorToWString(const std::vector<unsigned char>& vec)
{
    // Check if the vector size is valid for wide characters
    if (vec.size() % sizeof(wchar_t) != 0) {
        throw std::runtime_error("Invalid vector size for conversion to wstring");
    }

    // Create a wstring from the vector data
    return std::wstring(reinterpret_cast<const wchar_t*>(vec.data()), vec.size() / sizeof(wchar_t));
}
void WriteVectorToFile(const std::vector<unsigned char>& vec, const std::string& filename)
{
    // Open the file in binary write mode
    std::ofstream outFile(filename, std::ios::binary);

    // Check if the file is open
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return;
    }

    // Write the vector data to the file
    outFile.write(reinterpret_cast<const char*>(vec.data()), vec.size());

    // Close the file
    outFile.close();

    std::cout << "Successfully wrote to the file " << filename << std::endl;
}
void WriteVectorToTextFile(const std::vector<unsigned char>& vec, const std::string& filename)
{
    // Open the file in text write mode
    std::ofstream outFile(filename);

    // Check if the file is open
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return;
    }

    // Write the vector data to the file as text
    for (unsigned char ch : vec) {
        outFile << ch;
    }

    // Close the file
    outFile.close();

    std::cout << "Successfully wrote to the file " << filename << std::endl;
}

