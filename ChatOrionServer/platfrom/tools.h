#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>

class Tools
{
public:
     //char 转为16进制
    static unsigned char ToHex(unsigned char x);
    static unsigned char FromHex(unsigned char x);

    static std::string UrlEncode(const std::string& str);
    static std::string UrlDecode(const std::string& str);

    static std::string ReadFile(const std::string& file_path);

    // 去除字符串前后的空白字符（而不是中间的空白字符）
    static std::string Trim(const std::string& str);

    // 去除字符串中的所有空白字符
    static void RemoveWhitespace(std::string& str);

    static std::vector<std::string> GetLocalIPs();
private:
    Tools();
};

#endif // TOOLS_H
