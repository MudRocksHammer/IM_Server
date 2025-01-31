#include "BaseSocket.h"
#include "log.h"

typedef hash_map<net_handle_t, BaseSocket::ptr> SocketMap;
SocketMap g_socket_map;
static Logger::ptr g_logger = LOG_NAME("system");

void addBaseSocket(BaseSocket::ptr pSocket)
{
    g_socket_map.insert(std::make_pair((net_handle_t)pSocket->getSocket(), pSocket));
}

void removeBaseSocket(BaseSocket::ptr pSocket)
{
    g_socket_map.erase((net_handle_t)pSocket->getSocket());
}

BaseSocket::ptr findBaseSocket(net_handle_t fd)
{
    BaseSocket::ptr pSocket = nullptr;
    SocketMap::iterator it = g_socket_map.find(fd);
    if (it != g_socket_map.end())
    {
        pSocket = it->second;
    }

    return pSocket;
}

///------------------------------------------------------------------

BaseSocket::BaseSocket()
{
    m_socket = INVALID_SOCKET;
    m_state = SOCKET_State::SOCKET_STATE_IDLE;
}

BaseSocket::~BaseSocket()
{
}

void BaseSocket::setSendBufSize(U32_t send_size)
{
    int ret = setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &send_size, 4);
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "set SO_SNDBUF failed";
    }

    socklen_t len = 4;
    int size = 0;
    getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &size, &len);
    LOG_DEBUG(g_logger) << "socket send buf size";
}

void BaseSocket::setRecvBufSize(U32_t recv_size)
{
    int ret = setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, &recv_size, 4);
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "set SO_RCVBUF failed";
    }

    socklen_t len = 4;
    int size = 0;
    getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, &size, &len);
    LOG_DEBUG(g_logger) << "socket recv_buf_size";
}

int BaseSocket::listen(std::string server_ip, U16_t port, Callback_t callback, std::any data)
{
    m_local_ip = server_ip;
    m_local_port = port;
    m_callback = callback;
    m_callback_data = data;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET)
    {
        LOG_ERROR(g_logger) << "Socket failed, err_code = %d, server_ip = %s, port=%u";
        return NETLIB_FAIL;
    }

    _setReuseAddr(m_socket);
    _setNonBlock(m_socket);

    sockaddr_in serv_addr;
    int ret = ::bind(m_socket, (sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "Bind failed, err_code:";
        return NETLIB_FAIL;
    }

    ret = ::listen(m_socket, 64);
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "Listen failed, err_code";
    }

    m_state = SOCKET_State::SOCKET_STATE_LISTENING;

    LOG_DEBUG(g_logger) << "BaseSocket::Listening on";

    addBaseSocket(BaseSocket::ptr(this));
    // EventDispatch::getInstance()->addEvent(m_socket, SOCKET_READ | SOCKET_EXCEP);

    return NETLIB_OK;
}

net_handle_t BaseSocket::connect(std::string server_ip, U16_t port, Callback_t callback, std::any data)
{
    LOG_DEBUG(g_logger) << "BaseSocket::Connect, server_ip:";

    m_remote_ip = server_ip;
    m_remote_port = port;
    m_callback = callback;
    m_callback_data = data;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET)
    {
        LOG_ERROR(g_logger) << "Socket create failed";
        return NETLIB_INVALID_HANDLE;
    }

    _setNonBlock(m_socket);
    _setNoDelay(m_socket);

    sockaddr_in serv_addr;
    _setAddr(server_ip, port, &serv_addr);
    int ret = ::connect(m_socket, (sockaddr *)&serv_addr, sizeof(serv_addr));
    if ((ret == SOCKET_ERROR) && (!_isBlock(_getErrorCode())))
    {
        LOG_ERROR(g_logger) << "connect failed";
        closesocket(m_socket);
        return NETLIB_INVALID_HANDLE;
    }
    m_state = SOCKET_State::SOCKET_STATE_CONNECTING;
    addBaseSocket(BaseSocket::ptr(this));
    // EventDispatch::getInstance()->addEvent(m_socket, SOCKET_ALL);

    return net_handle_t(m_socket);
}

int BaseSocket::send(std::string data)
{
    if (m_state != SOCKET_State::SOCKET_STATE_CONNECTED)
        return NETLIB_FAIL;

    int ret = ::send(m_socket, data.c_str(), data.length(), 0);
    if (ret == SOCKET_ERROR)
    {
        int err_code = _getErrorCode();
        if (_isBlock(err_code))
        {
#if ((defined _WIN32) || (defined __APPLE__))
            // EventDispatch::getInstance()->addEvent(m_socket, SOCKET_WRITE);
#endif
            ret = 0;
        }
        else
        {
            LOG_ERROR(g_logger) << "send failed";
        }
    }
    return ret;
}

// TODO: maybe problem
std::string BaseSocket::recv()
{
    std::string recv_str;
    char buffer[RECEIVE_BUF_SIZE];
    memset(buffer, 0, RECEIVE_BUF_SIZE);

    int bytesReceived = ::recv(m_socket, buffer, RECEIVE_BUF_SIZE - 1, 0);
    if (bytesReceived > 0)
    {
        recv_str.append(buffer, bytesReceived);
    }

    return recv_str;
}

int BaseSocket::close()
{
    // EventDispatch::getInstance()->removeEvent(m_socket, SOCKET_ALL);
    removeBaseSocket(BaseSocket::ptr(this));
    closesocket(m_socket);
    return 0;
}

void BaseSocket::onRead()
{
    if (m_state == SOCKET_State::SOCKET_STATE_LISTENING)
    {
        _acceptNetSocket();
    }
    else
    {
        U64_t avail = 0;
        int ret = ioctlsocket(m_socket, FIONREAD, &avail);
        if ((SOCKET_ERROR == ret) || (avail == 0))
        {
            m_callback(m_callback_data, NETLIB_MSG_CLOSE, (net_handle_t)m_socket, std::any{});
        }
        else
        {
            m_callback(m_callback_data, NETLIB_MSG_READ, (net_handle_t)m_socket, std::any{});
        }
    }
}

void BaseSocket::onWrite()
{
#if ((defined _WIN32) || (defined __APPLE__))
    // EventDispatch::getInstance()->removeEvent(m_socket, SOCKET_WRITE);
#endif

    if (m_state == SOCKET_State::SOCKET_STATE_CONNECTING)
    {
        int error = 0;
        socklen_t len = sizeof(error);
#ifdef _WIN32
        getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char *)&error, &len);
#else
        getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void *)&error, &len);
#endif

        if (error)
        {
            m_callback(m_callback_data, NETLIB_MSG_CLOSE, (net_handle_t)m_socket, std::any{});
        }
        else
        {
            m_state = SOCKET_State::SOCKET_STATE_CONNECTED;
            m_callback(m_callback_data, NETLIB_MSG_CONFIRM, (net_handle_t)m_socket, std::any{});
        }
    }
    else
    {
        m_callback(m_callback_data, NETLIB_MSG_WRITE, (net_handle_t)m_socket, std::any{});
    }
}

void BaseSocket::onClose()
{
    m_state = SOCKET_State::SOCKET_STATE_CLOSING;
    m_callback(m_callback_data, NETLIB_MSG_CLOSE, (net_handle_t)m_socket, std::any{});
}

int BaseSocket::_getErrorCode()
{
#if _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool BaseSocket::_isBlock(int error_code)
{
#ifdef _WIN32
    return ((error_code == WSAEINPROGRESS) || (error_code == WSAEWOULDBLOCK));
#else
    return ((error_code == EINPROGRESS) || (error_code == EWOULDBLOCK));
#endif
    return false;
}

void BaseSocket::_setNonBlock(SOCKET fd)
{
#ifdef _WIN32
    U64_t nonblock = 1;
    int ret = ioctlsocket(fd, FIONBIO, &nonblock);
#else
    int ret = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL));
#endif
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "set NonBlock failed";
    }
}

void BaseSocket::_setReuseAddr(SOCKET fd)
{
    int reuse = 1;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "set reuse addr failed";
    }
}

void BaseSocket::_setNoDelay(SOCKET fd)
{
    int nodelay = -1;
    int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay, sizeof(nodelay));
    if (ret == SOCKET_ERROR)
    {
        LOG_ERROR(g_logger) << "set no delay failed";
    }
}

void BaseSocket::_setAddr(const std::string &ip, const U16_t port, sockaddr_in *pAddr)
{
    memset(pAddr, 0, sizeof(sockaddr_in));
    pAddr->sin_family = AF_INET;
    pAddr->sin_port = htons(port);
    pAddr->sin_addr.s_addr = inet_addr(ip.c_str());
    if (pAddr->sin_addr.s_addr == INADDR_NONE)
    {
        hostent *host = gethostbyname(ip.c_str());
        if (host = nullptr)
        {
            LOG_ERROR(g_logger) << "ge thost by name failed";
            return;
        }

        pAddr->sin_addr.s_addr = *(U32_t *)host->h_addr;
    }
}

void BaseSocket::_acceptNetSocket()
{
    SOCKET fd = 0;
    sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(sockaddr_in);
    char ip_str[64];
    while ((fd = accept(m_socket, (sockaddr *)&peer_addr, &addr_len)) != INVALID_SOCKET)
    {
        BaseSocket::ptr pSocket = std::make_shared<BaseSocket>();
        U32_t ip = ntohl(peer_addr.sin_addr.s_addr);
        U16_t port = ntohs(peer_addr.sin_port);

        snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

        LOG_DEBUG(g_logger) << "Accept new socket";

        pSocket->setSocket(fd);
        pSocket->setCallback(m_callback);
        pSocket->setState((int)SOCKET_State::SOCKET_STATE_CONNECTED);
        pSocket->setRemoteIP(ip_str);
        pSocket->setRemotePort(port);

        _setNoDelay(fd);
        _setNonBlock(fd);
        addBaseSocket(pSocket);

        // EventDispatch::getInstance()->addEvent(fd, SOCKET_READ | SOCKET_EXCEP);
        m_callback(m_callback_data, NETLIB_MSG_CONNECT, (net_handle_t)fd, std::any{});
    }
}
