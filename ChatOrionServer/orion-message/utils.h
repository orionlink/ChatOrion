//
// Created by hwk on 25-8-20.
//

#ifndef ORION_MESSAGE_UTILS_H
#define ORION_MESSAGE_UTILS_H

#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <chrono>
#include <random>

namespace message
{
    class Timestamp {
    public:
        Timestamp() {
            m_timePoint = std::chrono::system_clock::now();
        }

        std::string to_iso_string() const {
            auto in_time_t = std::chrono::system_clock::to_time_t(m_timePoint);
            char buffer[20];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", std::localtime(&in_time_t));
            return buffer;
        }

        time_t to_time_t() const {
            return std::chrono::system_clock::to_time_t(m_timePoint);
        }

        // 从time_t创建Timestamp
        static Timestamp from_time_t(time_t t) {
            Timestamp ts;
            ts.m_timePoint = std::chrono::system_clock::from_time_t(t);
            return ts;
        }

        // 从秒时间戳创建Timestamp
        static Timestamp from_seconds(uint64_t seconds) {
            Timestamp ts;
            std::chrono::seconds dur(seconds);
            ts.m_timePoint = std::chrono::system_clock::time_point(
                    std::chrono::duration_cast<std::chrono::system_clock::duration>(dur));
            return ts;
        }

        // 从毫秒时间戳创建Timestamp
        static Timestamp from_milliseconds(uint64_t milliseconds) {
            Timestamp ts;
            std::chrono::milliseconds dur(milliseconds);
            ts.m_timePoint = std::chrono::system_clock::time_point(
                    std::chrono::duration_cast<std::chrono::system_clock::duration>(dur));
            return ts;
        }

        // 从微秒时间戳创建Timestamp
        static Timestamp from_microseconds(uint64_t microseconds) {
            Timestamp ts;
            std::chrono::microseconds dur(microseconds);
            ts.m_timePoint = std::chrono::system_clock::time_point(
                    std::chrono::duration_cast<std::chrono::system_clock::duration>(dur));
            return ts;
        }

        /**
         * 返回毫秒时间戳
         * @return
         */
        uint64_t to_milliseconds() const {
            auto duration = m_timePoint.time_since_epoch();
            return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        }

        /**
         * 毫秒时间戳
         * @return
         */
        uint64_t to_seconds() const {
            auto duration = m_timePoint.time_since_epoch();
            return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        }

        /**
         * 微秒时间戳
         * @return
         */
        uint64_t to_microseconds() const {
            auto duration = m_timePoint.time_since_epoch();
            return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        }

        void from_iso_string(const std::string& iso_str) {
            std::tm tm = {};
            std::istringstream ss(iso_str);
            ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
            m_timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }

    private:
        std::chrono::system_clock::time_point m_timePoint;
    };
}

#endif //ORION_MESSAGE_UTILS_H
