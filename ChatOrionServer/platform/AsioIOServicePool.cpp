//
// Created by hwk on 2025/1/4.
//

#include "AsioIOServicePool.h"

#include <memory>
#include <iostream>

AsioIOServicePool::AsioIOServicePool(int pool_size)
    :_io_service(pool_size), _work_ptrs(pool_size),_next_ioService(0)
{
    for (int i = 0; i < pool_size; i++)
    {
        _work_ptrs[i] = std::make_unique<Work>(boost::asio::make_work_guard(_io_service[i]));
    }

    for (int i = 0; i < _io_service.size(); i++)
    {
        _threads.emplace_back([this, i]()
        {
            std::cout << "thread id: " << std::this_thread::get_id() << " start" << std::endl;
            _io_service[i].run();
        });
    }
}

AsioIOServicePool::~AsioIOServicePool()
{
    stop();
}

void AsioIOServicePool::stop()
{
    for (auto& io_service : _io_service) {
        io_service.stop();
    }
    _work_ptrs.clear();  // 清除 work guard
    for (auto& t : _threads) {
        t.join();
    }
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
    auto& io_service =  _io_service[_next_ioService++];
    if (_next_ioService == _io_service.size())
        _next_ioService = 0;

    return io_service;
}
