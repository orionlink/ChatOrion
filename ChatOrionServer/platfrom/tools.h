#ifndef TOOLS_H
#define TOOLS_H

#include <string>

class Tools
{
public:
     //char 转为16进制
    static unsigned char ToHex(unsigned char x);
    static unsigned char FromHex(unsigned char x);

    static std::string UrlEncode(const std::string& str);
    static std::string UrlDecode(const std::string& str);

    static std::string ReadFile(const std::string& file_path);

    // 去除字符串前后的空白字符
    static std::string Trim(const std::string& str);
private:
    Tools();
};

#endif // TOOLS_H
