#pragma once

class Noncopyble
{
public:
    Noncopyble() = default;
    ~Noncopyble() = default;
    // 禁止拷贝构造和赋值拷贝构造
    Noncopyble(const Noncopyble &) = delete;
    Noncopyble &operator=(const Noncopyble &) = delete;
};
