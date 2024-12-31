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
private:
    Tools();
};

#endif // TOOLS_H
