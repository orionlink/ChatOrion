//
// Created by hwk on 2025/1/4.
//

#ifndef ASIOIOSERVICEPOOL_H
#define ASIOIOSERVICEPOOL_H

#include "singleton.hpp"
#include "const.h"

#include <vector>
#include <thread>
#include <mutex>
#include <boost/asio/io_context.hpp>

class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
    friend class Singleton<AsioIOServicePool>;
public:
    /**
     * 该IO_CONTEXT如果没有绑定监听任何的读写事件和接受连接事件，run 起来则立马退出
     * Work 保证事件循环
     */
    using IOService = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;

    ~AsioIOServicePool();

    void stop();

    boost::asio::io_context& GetIOService();
private:
    AsioIOServicePool(int pool_size = std::thread::hardware_concurrency());

    std::vector<IOService> _io_service;
    std::vector<WorkPtr> _work_ptrs;
    std::vector<std::thread> _threads;
    int _next_ioService;
};

#endif //ASIOIOSERVICEPOOL_H
