//
// Created by hwk on 2025/2/12.
//

#ifndef GLOGWRAPPER_H
#define GLOGWRAPPER_H

#include <glog/logging.h>
#include <string>
#include <memory>
#include <fstream>

#include "singleton.hpp"

#define LOG_INFO    LOG(INFO)
#define LOG_WARNING LOG(WARNING)
#define LOG_ERROR   LOG(ERROR)
#define LOG_FATAL   LOG(FATAL)

// 终端颜色定义
class TermColor {
public:
    static constexpr const char* RESET      = "\033[0m";
    static constexpr const char* RED        = "\033[31m";
    static constexpr const char* GREEN      = "\033[32m";
    static constexpr const char* YELLOW     = "\033[33m";
    static constexpr const char* BLUE       = "\033[34m";
    static constexpr const char* MAGENTA    = "\033[35m";
    static constexpr const char* CYAN       = "\033[36m";
    static constexpr const char* WHITE      = "\033[37m";
    static constexpr const char* BOLD_RED   = "\033[1;31m"; // 加粗红色
    static constexpr const char* BG_RED     = "\033[41m";
    static constexpr const char* BOLD       = "\033[1m";
};

class CustomLogSink;

class Log : public Singleton<Log>
{
    friend class Singleton<Log>;
public:
    /**
     * 初始化glog
     * @param program_name 程序名称
     * @param singleLog true - 每天生成一个log, false - 按照运行时间点生成log
     * @param log_dir 日志目录
     * @param log_to_stderr 是否输出到标准错误
     * @param v 日志级别, 0-info，1-WARNING，2-ERROR，3-FATAL
     * @return 是否初始化成功
     */
    bool Initialize(const std::string& program_name = "",
                    bool singleLog = true,
                    const std::string& log_dir = "./logs",
                    bool log_to_stderr = true,
                    int v = 0);

    // 关闭glog
    void Shutdown();

    // 析构函数中确保关闭glog
    ~Log() {
        Shutdown();
    }

private:
    Log() : initialized_(false) {}

    // 创建日志目录
    bool CreateLogDirectory(const std::string& log_dir);

    std::string GenerateLogFileName();

    bool initialized_;  // 标记是否已初始化
    CustomLogSink* custom_sink;
};

class CustomLogSink : public google::LogSink {

public:
    CustomLogSink(const std::string& base_filename);

    void send(google::LogSeverity severity, const char* full_filename,
        const char* base_filename, int line,
        const struct ::tm* tm_time,
        const char* message, size_t message_len) override;

private:
    void UpdateLogFile();
    std::string GetCurrentTimeWithMilliseconds();

private:
    std::ofstream log_file_;
    std::string base_filename_;
    std::mutex mutex_;
    int last_day_ = -1;

};

#endif //GLOGWRAPPER_H
