#ifndef __DY_SINGLETON__
#define __DY_SINGLETON__
#include <iostream>
#include <memory>

namespace dysv{
    /**
     * @brief 简单封装实现单例。Singleton::GetInstance()返回原生指针。
     * 
     * @tparam T 
     */
    template<class T>
    class Singleton{
    public:
        static T* GetInstance(){
            static T ins;
            return &ins;
        }
    };


    template<class T>
    class SingletonPtr{
    public:
        static std::shared_ptr<T> GetInstance(){
            static std::shared_ptr<T> ins(new T);
            return ins;
        }
    };
} // namespace dysv

#endif