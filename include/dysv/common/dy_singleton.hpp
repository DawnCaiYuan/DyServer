#pragma once
#include <iostream>
#include <memory>

namespace dysv{
    /**
     * @brief 简单封装实现单例。Singleton::GetInstance()返回原生指针。
     * 
     * @tparam T 
     */
    template<class T>
    class Singleton final{
    public:
        static T* GetInstance(){
            static T ins;
            return &ins;
        }
    private:
        Singleton() = default;
    };

    template<class T>
    class SingletonPtr final{
    public:
        static std::shared_ptr<T> GetInstance(){
            static auto ins = std::make_shared<T>();
            return ins;
        }
    private:
        SingletonPtr() = default;
    };
} // namespace dysv