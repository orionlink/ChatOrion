#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>
#include <chrono>

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

    static std::string timeToString(time_t timeVal);

    static int64_t stringToTimestamp(const std::string& timeStr);

     /**
      * 生成唯一的uuid字符串
      * @return
      */
     static std::string generate_unique_string();

    // 定义时间精度枚举
     enum class TimeUnit {
         SECONDS,      // 秒
         MILLISECONDS, // 毫秒
         MICROSECONDS, // 微秒
         NANOSECONDS   // 纳秒
     };

    // 获取当前时间戳的通用函数
    template<typename T = int64_t>
    static T getCurrentTimestamp(TimeUnit unit = TimeUnit::SECONDS)
    {
        using namespace std::chrono;
        auto now = system_clock::now();

        switch (unit) {
            case TimeUnit::SECONDS:
                return static_cast<T>(duration_cast<seconds>(now.time_since_epoch()).count());
            case TimeUnit::MILLISECONDS:
                return static_cast<T>(duration_cast<milliseconds>(now.time_since_epoch()).count());
            case TimeUnit::MICROSECONDS:
                return static_cast<T>(duration_cast<microseconds>(now.time_since_epoch()).count());
            case TimeUnit::NANOSECONDS:
                return static_cast<T>(duration_cast<nanoseconds>(now.time_since_epoch()).count());
            default:
                return static_cast<T>(0);
        }
    }

    // 便捷函数 - 获取秒级时间戳
    static int64_t getCurrentTimestampInSeconds() {
        return getCurrentTimestamp(TimeUnit::SECONDS);
    }

    // 便捷函数 - 获取毫秒级时间戳
    static int64_t getCurrentTimestampInMilliseconds() {
        return getCurrentTimestamp(TimeUnit::MILLISECONDS);
    }

    // 便捷函数 - 获取微秒级时间戳
    static int64_t getCurrentTimestampInMicroseconds() {
        return getCurrentTimestamp(TimeUnit::MICROSECONDS);
    }

    // 便捷函数 - 获取纳秒级时间戳
    static int64_t getCurrentTimestampInNanoseconds() {
        return getCurrentTimestamp(TimeUnit::NANOSECONDS);
    }
private:
    Tools();
};

#endif // TOOLS_H
