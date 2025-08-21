#ifndef SINGLETON_H
#define SINGLETON_H
/******************************************************************************
 *
 * @file       singleton.h
 * @brief      单例模版类 Function
 *
 * @author     hwk
 * @date       2024/12/27
 * @history
 *****************************************************************************/

#include <memory>
#include <mutex>
#include <iostream>

template<class T>
class Singleton
{
public:
    static std::shared_ptr<T> GetInstance()
    {
        static std::once_flag flag;
        std::call_once(flag, [&]
        {
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddress()
    {
        std::cout << _instance.get() << std::endl;
    }

    virtual ~Singleton()
    {
        std::cout << "this is singleton destruct" << std::endl;
    }
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) =delete;
    Singleton<T>& operator=(const Singleton<T>&) = delete;
private:
    static std::shared_ptr<T> _instance;
};

template<class T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif // SINGLETON_H
