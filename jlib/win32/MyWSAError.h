#pragma once

#include <WinSock2.h>
#include "UnicodeTool.h"

namespace jlib
{
namespace win32
{

inline const wchar_t* FormatWSAError(int errornumber)
{
	switch (errornumber) {
	case WSANOTINITIALISED:
		return L"A successful WSAStartup call must occur before using this function. ";
	case WSAENETDOWN:
		return L"The network subsystem has failed. ";
	case WSAEACCES:
		return L"The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address. ";
	case WSAEINVAL:
		return L"An unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. ";
	case WSAEINTR:
		return L"A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. ";
	case WSAEINPROGRESS:
		return L"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";
	case WSAEFAULT:
		return L"The buf or to parameters are not part of the user address space, or the tolen parameter is too small. ";
	case WSAENETRESET:
		return L"The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. ";
	case WSAENOBUFS:
		return L"No buffer space is available. ";
	case WSAENOTCONN:
		return L"The socket is not connected (connection-oriented sockets only. ";
	case WSAENOTSOCK:
		return L"The descriptor is not a socket. ";
	case WSAEOPNOTSUPP:
		return L"MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. ";
	case WSAESHUTDOWN:
		return L"The socket has been shut down; it is not possible to sendto on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. ";
	case WSAEWOULDBLOCK:
		return L"The socket is marked as nonblocking and the requested operation would block. ";
	case WSAEMSGSIZE:
		return L"The socket is message oriented, and the message is larger than the maximum supported by the underlying transport. ";
	case WSAEHOSTUNREACH:
		return L"The remote host cannot be reached from this host at this time. ";
	case WSAECONNABORTED:
		return L"The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. ";
	case WSAECONNRESET:
		return L"The virtual circuit was reset by the remote side executing a hard or abortive close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable. ";
	case WSAEADDRNOTAVAIL:
		return L"The remote address is not a valid address, for example, ADDR_ANY. ";
	case WSAEAFNOSUPPORT:
		return L"Addresses in the specified family cannot be used with this socket. ";
	case WSAEDESTADDRREQ:
		return L"A destination address is required. ";
	case WSAENETUNREACH:
		return L"The network cannot be reached from this host at this time. ";
	case WSAETIMEDOUT:
		return L"The connection has been dropped, because of a network failure or because the system on the other end went down without notice. ";
	default:
	{
		static wchar_t buf[1024] = { 0 };
		swprintf_s(buf, L"Unknown socket error %d.", errornumber);
		return buf;
	}
	}
}

inline const char* FormatWSAErrorA(int errornumber)
{
	return utf16_to_mbcs(FormatWSAError(errornumber)).c_str();
}

}
}
