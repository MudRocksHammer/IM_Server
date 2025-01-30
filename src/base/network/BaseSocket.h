#pragma once

#include "ostype.h"

enum class SOCKET_State
{
    SOCKET_STATE_IDLE,
    SOCKET_STATE_LISTENING,
    SOCKET_STATE_CONNECTING,
    SOCKET_STATE_CONNECTED,
    SOCKET_STATE_CLOSING
};

#define RECEIVE_BUF_SIZE 1024

class BaseSocket
{
public:
    typedef std::shared_ptr<BaseSocket> ptr;
    BaseSocket();

    virtual ~BaseSocket();

    SOCKET getSocket() { return m_socket; }
    void setSocket(SOCKET fd) { m_socket = fd; }
    void setState(U8_t state) { m_state = (SOCKET_State)state; }

    void setCallback(Callback_t callback) { m_callback = callback; }
    void setCallbackData(std::any data) { m_callback_data = data; }
    void setRemoteIP(const std::string &ip) { m_remote_ip = ip; }
    void setRemotePort(U16_t port) { m_remote_port = port; }
    void setSendBufSize(U32_t send_size);
    void setRecvBufSize(U32_t recv_size);

    const std::string getRemoteIP() const { return m_remote_ip; }
    const U16_t getRemotePort() const { return m_remote_port; }
    const std::string getLocalIP() const { return m_local_ip; }
    const U16_t getLocalPort() const { return m_local_port; }

    int listen(std::string server_ip, U16_t port, Callback_t callback, std::any data);
    net_handle_t connect(std::string server_ip, U16_t port, Callback_t callback, std::any data);
    int send(std::string data);
    std::string recv();
    int close();

    void onRead();
    void onWrite();
    void onClose();

private:
    int _getErrorCode();
    bool _isBlock(int error_code);

    void _setNonBlock(SOCKET fd);
    void _setReuseAddr(SOCKET fd);
    void _setNoDelay(SOCKET fd);
    void _setAddr(const std::string &ip, const U16_t port, sockaddr_in *pAddr);

    void _acceptNetSocket();

private:
    SOCKET m_socket; // sockfd
    SOCKET_State m_state;

    // bind ip address
    std::string m_remote_ip;
    U16_t m_remote_port;
    std::string m_local_ip;
    U16_t m_local_port;

    Callback_t m_callback;
    std::any m_callback_data;
};