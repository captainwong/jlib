#pragma once
namespace jlib
{
	inline const wchar_t* FormatWSAError(int errornumber)
	{
		switch (errornumber) {
			case WSANOTINITIALISED:
				return L"A successful WSAStartup call must occur before using this function. ";
				break;
			case WSAENETDOWN:
				return L"The network subsystem has failed. ";
				break;
			case WSAEACCES:
				return L"The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address. ";
				break;
			case WSAEINVAL:
				return L"An unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. ";
				break;
			case WSAEINTR:
				return L"A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. ";
				break;
			case WSAEINPROGRESS:
				return L"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";
				break;
			case WSAEFAULT:
				return L"The buf or to parameters are not part of the user address space, or the tolen parameter is too small. ";
				break;
			case WSAENETRESET:
				return L"The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. ";
				break;
			case WSAENOBUFS:
				return L"No buffer space is available. ";
				break;
			case WSAENOTCONN:
				return L"The socket is not connected (connection-oriented sockets only. ";
				break;
			case WSAENOTSOCK:
				return L"The descriptor is not a socket. ";
				break;
			case WSAEOPNOTSUPP:
				return L"MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. ";
				break;
			case WSAESHUTDOWN:
				return L"The socket has been shut down; it is not possible to sendto on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. ";
				break;
			case WSAEWOULDBLOCK:
				return L"The socket is marked as nonblocking and the requested operation would block. ";
				break;
			case WSAEMSGSIZE:
				return L"The socket is message oriented, and the message is larger than the maximum supported by the underlying transport. ";
				break;
			case WSAEHOSTUNREACH:
				return L"The remote host cannot be reached from this host at this time. ";
				break;
			case WSAECONNABORTED:
				return L"The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. ";
				break;
			case WSAECONNRESET:
				return L"The virtual circuit was reset by the remote side executing a hard or abortive close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable. ";
				break;
			case WSAEADDRNOTAVAIL:
				return L"The remote address is not a valid address, for example, ADDR_ANY. ";
				break;
			case WSAEAFNOSUPPORT:
				return L"Addresses in the specified family cannot be used with this socket. ";
				break;
			case WSAEDESTADDRREQ:
				return L"A destination address is required. ";
				break;
			case WSAENETUNREACH:
				return L"The network cannot be reached from this host at this time. ";
				break;
			case WSAETIMEDOUT:
				return L"The connection has been dropped, because of a network failure or because the system on the other end went down without notice. ";
				break;
			default:{
				static wchar_t buf[1024] = { 0 };
				swprintf_s(buf, L"Unknown socket error %d.", errornumber);
				return buf;
			}break;
		}
	}

};
