#include "netlib.h"

NETLIB::ptr NETLIB::getInstance()
{
    static std::once_flag init_flag;
    std::call_once(init_flag, []()
                   { m_netlib = std::make_shared<NETLIB>(); });
    return m_netlib;
}