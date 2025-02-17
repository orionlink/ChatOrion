//
// Created by hwk on 2025/2/12.
//

#include "log.h"

#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <codecvt>
#include <ctime>

bool Log::Initialize(const std::string &program_name, bool singleLog, const std::string &log_dir, bool log_to_stderr,
    int v)
{
    try {
        // 确保日志目录存在
        if (!CreateLogDirectory(log_dir)) {
            return false;
        }

        // 设置日志目录
        FLAGS_log_dir = log_dir;

        // 关闭Glog的默认终端输出，完全由CustomLogSink处理
        FLAGS_logtostderr = false;
        FLAGS_alsologtostderr = false;
        FLAGS_colorlogtostderr = false; // 禁用Glog默认颜色，使用自定义颜色

        // 初始化glog
        google::InitGoogleLogging("");

        // 设置日志文件大小（20MB）
        FLAGS_max_log_size = 512;

        // 设置日志保留策略
        FLAGS_logbufsecs = 0;                   // 立即刷新
        FLAGS_stop_logging_if_full_disk = true; // 磁盘满时停止

        initialized_ = true;

        // 查找最后一个反斜杠的位置，它之后的就是进程名
        std::string::size_type pos = std::string(program_name).find_last_of("\\/");
        std::string program_name_new = std::string(program_name).substr(pos + 1);

        if (singleLog)
        {
            // 禁用默认的日志输出
            google::SetLogDestination(google::GLOG_INFO, "");
            google::SetLogDestination(google::GLOG_WARNING, "");
            google::SetLogDestination(google::GLOG_ERROR, "");
            google::SetLogDestination(google::GLOG_FATAL, "");

            std::string filename = log_dir + "/" + program_name_new + "-"+ GenerateLogFileName();
            custom_sink = new CustomLogSink(filename);
            google::AddLogSink(custom_sink);
        }
        else
        {
            std::string info_log_path = log_dir + "/INFO_" + program_name_new;
            std::string warn_log_path = log_dir + "/WARNING_" + program_name_new;
            std::string error_log_path = log_dir + "/ERROR_" + program_name_new;

            // 设置特定严重级别的日志的输出目录和前缀
            // 第一个参数为日志级别，第二个参数表示输出目录及日志文件名前缀

            google::SetLogDestination(google::GLOG_INFO, info_log_path.c_str());

            google::SetLogDestination(google::GLOG_WARNING, warn_log_path.c_str());

            google::SetLogDestination(google::GLOG_ERROR, error_log_path.c_str());

        }

        LOG_INFO << "GLog initialized successfully in directory: " << log_dir;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "\033[31mFailed to initialize GLog: "
                 << e.what() << "\033[0m" << std::endl;
        return false;
    }
}

void Log::Shutdown()
{
    if (initialized_) {
        google::ShutdownGoogleLogging();
        initialized_ = false;
    }
}

bool Log::CreateLogDirectory(const std::string &log_dir)
{
    try {
        if (log_dir.empty()) {
            return false;
        }

        // 使用系统命令创建目录（支持递归创建）
        std::string cmd = "mkdir -p " + log_dir;
        int ret = system(cmd.c_str());
        return (ret == 0);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create log directory: " << e.what() << std::endl;
        return false;
    }
}

std::string Log::GenerateLogFileName()
{
    // 获取当前时间
    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::localtime(&now);

    // 生成基于日期的日志文件名
    std::ostringstream oss;

    oss << (now_tm->tm_year + 1900) << "-"
        << std::setfill('0') << std::setw(2) << (now_tm->tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << now_tm->tm_mday << ".log";

    return oss.str();
}



CustomLogSink::CustomLogSink(const std::string& base_filename) : base_filename_(base_filename)
{
    UpdateLogFile();
}

void CustomLogSink::send(google::LogSeverity severity, const char* full_filename,
    const char* base_filename, int line,
    const struct ::tm* tm_time,
    const char* message, size_t message_len)
{
    std::lock_guard<std::mutex> lock(mutex_);
    UpdateLogFile();

    // 获取文件名（不要完整路径）
    const char* filename = base_filename ? base_filename : full_filename;

    // 构造位置信息
    std::string location = std::string(filename) + ":" + std::to_string(line);

    // 构造格式化的日志消息（包括时间、级别、位置和用户消息）
    std::string severity_name = google::GetLogSeverityName(severity);
    std::string time_str = GetCurrentTimeWithMilliseconds();
    std::string formatted_message =
        "[" + severity_name + "] " + time_str + " " +
        location + ": " + std::string(message, message_len) + "\n";

    // 写入文件（无颜色）
    if (log_file_.is_open()) {
        log_file_ << formatted_message << std::flush;
    }

    // 设置颜色
    const char* color = TermColor::RESET;
    switch (severity) {
        case google::GLOG_INFO:
            color = TermColor::GREEN;
        break;
        case google::GLOG_WARNING:
            color = TermColor::YELLOW;
        break;
        case google::GLOG_ERROR:
            color = TermColor::RED;
        break;
        case google::GLOG_FATAL:
            color = TermColor::BOLD_RED;
        break;
        default:
            break;
    }

    // 输出带颜色的日志到终端
    std::cerr << color << formatted_message << TermColor::RESET;
}

void CustomLogSink::UpdateLogFile() {

    time_t now = time(nullptr);

    struct tm now_tm;

#ifdef _WIN32
    localtime_s(&now_tm, &now);
#else
    localtime_r(&now, &now_tm);
#endif

    if (now_tm.tm_mday != last_day_)
    {
        if (log_file_.is_open())
        {
            log_file_.close();
        }

        std::string log_filename = base_filename_;
        log_file_.open(log_filename, std::ios_base::app);
        last_day_ = now_tm.tm_mday;
    }
}

std::string CustomLogSink::GetCurrentTimeWithMilliseconds() {

    // 获取当前系统时间点
    auto now = std::chrono::system_clock::now();

    // 转换为time_t时间类型
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);

    // 转换为tm结构
    struct tm timeinfo;

#ifdef _WIN32
    localtime_s(&timeinfo, &now_as_time_t);
#else
    localtime_r(&now_as_time_t, &timeinfo);
#endif

    // 获取当前时间的毫秒部分
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(

        now.time_since_epoch()) % 1000;

    // 使用wstringstream进行格式化
    std::wstringstream wss;

    wss << std::put_time(&timeinfo, L"%Y-%m-%d %H:%M:%S")
        << L'.' << std::setfill(L'0') << std::setw(3) << milliseconds.count();

    std::wstring wstr = wss.str();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    return converter.to_bytes(wstr);
}
