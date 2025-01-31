#pragma once

#include "ostype.h"
#include "singleton.h"

enum class NETLIB_OPT
{
    NETLIB_OPT_SET_CALLBACK = 1,
    NETLIB_OPT_SET_CALLBACK_DATA,
    NETLIB_OPT_GET_REMOTE_IP,
    NETLIB_OPT_GET_REMOTE_PORT,
    NETLIB_OPT_GET_LOCAL_IP,
    NETLIB_OPT_GET_LOCAL_PORT,
    NETLIB_OPT_SET_SEND_BUG_SIZE,
    NETLIB_OPT_SET_RECV_BUF_SIZE,
};

#define NETLIB_MAX_SOCKET_BUF_SIZE (128 * 1024)

class NETLIB
{
public:
    typedef std::shared_ptr<NETLIB> ptr;
    static NETLIB::ptr getInstance();

    int netlibListen(std::string server_ip, U16_t port, Callback_t callback, std::any callback_data);
    net_handle_t netlibConnect(std::string server_ip, U16_t port, Callback_t callback, std::any callback_data);
    int netlibSend(net_handle_t handle, std::string send_data);
    int netlibRecv(net_handle_t handle, std::string recv_data);
    int netlibClose(net_handle_t handle);
    int netlibOption(net_handle_t handle);
    int netlibRegisterTimer(Callback_t callback, std::any user_data, U64_t interval);

private:
    NETLIB();
    NETLIB(const NETLIB &) = delete;
    NETLIB &operator=(const NETLIB &) = delete;
    NETLIB(NETLIB &&) = delete;
    NETLIB &operator=(NETLIB &&) = delete;

    int netlibInit();

private:
    static NETLIB::ptr m_netlib;
};