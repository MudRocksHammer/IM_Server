#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WinBase.h>
#include <Windows.h>
#include <direct.h>
#else

#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#include <sys/syscall.h>
#else
#include <sys/epoll.h>
#endif //__APPLE__

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <cstdarg>

#define closesocket ::close
#define ioctlsocket ::ioctl

#endif // WIN32

#include <stdexcept>
#include <functional>
#include <any>

#ifdef __GNUC__

#include <ext/hash_map>
using namespace __gnu_cxx;

namespace __gnu_cxx
{
    template <>
    struct hash<std::string>
    {
        size_t operator()(const std::string &x) const
        {
            return hash<const char *>()(x.c_str());
        }
    };
}

#else
#include <hash_mp>
using namespace stdext;

#endif //__GNUC__

#ifdef _WIN32
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef int socklen_t;

#else
typedef int SOCKET;
typedef int BOOL;

#ifdef __APPLE__
const int TRUE = 1;
const int FALSE = 0;
#endif //__APPLE__

const int SOCKET_ERROR = -1;
const int INVALID_SOCKET = -1;

#endif //_WIN32

typedef unsigned char uchar_t;
// net file descriptor
typedef int net_handle_t;
// net connection file descriptor
typedef int conn_handle_t;

enum
{
    NETLIB_OK = 0,
    NETLIB_FAIL = -1
};

#define NETLIB_INVALID_HANDLE -1

enum
{
    NETLIB_MSG_CONNECT = 1,
    NETLIB_MSG_CONFIRM,
    NETLIB_MSG_READ,
    NETLIB_MSG_WRITE,
    NETLIB_MSG_CLOSE,
    NETLIB_MSG_TIMER,
    NETLIB_MSG_LOOP
};

const uint32_t INVALID_UINT32 = (uint32_t)-1;
const uint32_t INVALID_VALUE = 0;

#define U8_t unsigned char
#define I8_t char
#define U16_t unsigned short
#define I16_t short
#define U32_t uint32_t
#define I32_t int32_t
#define U64_t uint64_t
#define I64_t int64_t

/// @brief declare a function pointer that receive 4 parameters as Callback_t
typedef std::function<void(const std::any &callback_data, U8_t msg, U32_t handle, const std::any &param)> Callback_t;