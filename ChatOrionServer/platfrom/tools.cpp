#include "tools.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/asio.hpp>
#include <iomanip>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

Tools::Tools()
{

}

unsigned char Tools::ToHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char Tools::FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

std::string Tools::UrlEncode(const std::string &str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        //判断是否仅有数字和字母构成
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else
        {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] & 0x0F);
        }
    }
    return strTemp;
}

std::string Tools::UrlDecode(const std::string &str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
        //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}

std::string Tools::ReadFile(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open SQL file: " + file_path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Tools::Trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

void Tools::RemoveWhitespace(std::string &str)
{
    /**
    *std::remove_if：遍历字符串，将所有空白字符移动到字符串的末尾。返回一个指向新逻辑结尾的迭代器。
    *
    *str.erase：删除从新逻辑结尾到字符串实际结尾之间的所有字符（即所有空白字符）。
     */
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c)
    {
        return std::isspace(c);
    }), str.end());
}

std::vector<std::string> Tools::GetLocalIPs()
{
    std::vector<std::string> result;
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::udp::socket socket(io_context);

        // 通过连接到一个公共IP（这里是8.8.8.8）来获取本地IP
        socket.open(boost::asio::ip::udp::v4());
        socket.connect(boost::asio::ip::udp::endpoint(boost::asio::ip::make_address("8.8.8.8"), 53));

        auto local_endpoint = socket.local_endpoint();
        auto local_address = local_endpoint.address();
        result.push_back(local_address.to_string());

        socket.close();

    } catch (const std::exception& e) {
        std::cerr << "Error in GetLocalIPs: " << e.what() << std::endl;
    }

    return result;
}

std::string Tools::timeToString(time_t timeVal)
{
    if (timeVal <= 0) {
        return "1970-01-01 00:00:00"; // Or handle invalid timestamp differently
    }

    std::tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &timeVal);
#else
    localtime_r(&timeVal, &timeInfo);
#endif

    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    return std::string(buffer);
}

int64_t Tools::stringToTimestamp(const std::string& timeStr)
{
    struct tm tm_time = {0};

#ifdef _WIN32
    // Windows 版本
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return 0;
    }
#else
    // Linux 版本
    if (!strptime(timeStr.c_str(), "%Y-%m-%d %H:%M:%S", &tm_time)) {
        return 0;
    }
#endif

    tm_time.tm_isdst = -1;
    return static_cast<int64_t>(mktime(&tm_time));
}

std::string Tools::generate_unique_string()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);

    return unique_string;
}
