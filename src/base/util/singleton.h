#pragma once

#include <memory>

template <class T, class X, int N>
T &getInstanceX()
{
    static T v;
    return v;
}

template <class T, class X, int N>
std::shared_ptr<T> getInstancePtr()
{
    static std::shared_ptr<T> v(new T);
    return v;
}

/**
 * @brief 单例模式封装类
 * @details T 类型
 *          X 为了创建多个实例对应的tag
 *          N 同一个tag创建多个实例索引
 */
template <class T, class X = void, int N = 0>
class Singleton
{
public:
    /**
     * @brief 返回单例指针
     */
    static T *getInstance()
    {
        static T v;
        return &v;
    }
};

/**
 * @brief 单例模式智能指针封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个tag创造多个实例索引
 */
template <class T, class X = void, int N = 0>
class SingletonPtr
{
public:
    /**
     * @brief 返回单例智能指针
     */
    static std::shared_ptr<T> getInstance()
    {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};