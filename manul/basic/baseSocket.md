# [BaseSocket](/src/base/network/BaseSocket.h#L14)

## Introduction 

This class is an encapsulation of net work socket.

## public types

### enum SOCKET_State
|Constant|value|description|
|:------|:------|:---|
| SOCKET_STATE_IDLE|0|socket闲置状态|
| SOCKET_STATE_LISTENING|1|socket监听状态|
| SOCKET_STATE_CONNECTING|2|socket正在连接状态|
| SOCKET_STATE_CONNECTED|3|socket已连接状态|
| SOCKET_STATE_CLOSE|4|socket关闭状态|

## relative type

### [NETLIB_MSG_STATE](/src/base/util/ostype.h#L103)

|Constant|value|description|
|:---|:---|:---|
|NETLIB_MSG_CONNECT|1||
|NETLIB_MSG_CONFIRM|2||
|NETLIB_MSG_READ|3||
|NETLIB_MSG_WRITE|4||
|NETLIB_MSG_CLOSE|5||
|NETLIB_MSG_TIMER|6||
|NETLIB_MSG_LOOP|7||

## properties

|name|type|description|
|:------:|:------|:--|
|m_remote_ip | string | |
|m_remote_port| Uint16 | |
|m_local_ip|string||
|m_local_port|uint16||
|m_callback|Callback_t|回调函数 typedef std::function<void(const std::any &callback_data, U8_t msg, U32_t handle, const std::any &param)> Callback_t|
|m_callback_data|std::any|回调函数数据,任意类型的数据|
|m_socket|SOCKET|socket fd; typedef int SOCKET;|
|m_state|[SOCKET_State](#enum-socket_state)|socket状态枚举|

## public function

