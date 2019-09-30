#pragma once

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "../log2.h"

namespace jlib
{
namespace win32
{

//! get first ipv4 of domain
static std::string resolve(const std::string& domain)
{
	AUTO_LOG_FUNCTION;
	struct addrinfo* result = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints;

	struct sockaddr_in* sockaddr_ipv4;
	//    struct sockaddr_in6 *sockaddr_ipv6;
	LPSOCKADDR sockaddr_ip;

	char ipstringbuffer[46];
	DWORD ipbufferlength = 46;

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	std::string ip;

	do {

		//--------------------------------
		// Call getaddrinfo(). If the call succeeds,
		// the result variable will hold a linked list
		// of addrinfo structures containing response
		// information
		DWORD dwRetval = getaddrinfo(domain.c_str(), "0", &hints, &result);
		if (dwRetval != 0) {
			JLOG_INFO("getaddrinfo failed with error: {}", dwRetval);
			break;
		}

		JLOG_INFO("getaddrinfo returned success");

		// Retrieve each address and print out the hex bytes
		int i = 0;
		int iRetval = 0;
		bool ok = false;
		for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
			JLOG_INFO("getaddrinfo response {}", i++);
			JLOG_INFO("\tFlags: 0x{:08x}", ptr->ai_flags);
			JLOG_INFO("\tFamily: ");
			switch (ptr->ai_family) {
			case AF_UNSPEC:
				JLOG_INFO("Unspecified");
				break;
			case AF_INET:
				JLOG_INFO("AF_INET (IPv4)");
				sockaddr_ipv4 = (struct sockaddr_in*) ptr->ai_addr;
				ip = inet_ntoa(sockaddr_ipv4->sin_addr);
				JLOG_INFO("\tIPv4 address {}", ip);
				ok = true;
				break;
			case AF_INET6:
				JLOG_INFO("AF_INET6 (IPv6)");
				// the InetNtop function is available on Windows Vista and later
				// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				// printf("\tIPv6 address %s",
				//    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

				// We use WSAAddressToString since it is supported on Windows XP and later
				sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
				// The buffer length is changed by each call to WSAAddresstoString
				// So we need to set it for each iteration through the loop for safety
				ipbufferlength = 46;
				iRetval = WSAAddressToStringA(sockaddr_ip, (DWORD)ptr->ai_addrlen, nullptr,
											  ipstringbuffer, &ipbufferlength);
				if (iRetval)
					JLOG_INFO("WSAAddressToString failed with {}", WSAGetLastError());
				else
					JLOG_INFO("\tIPv6 address {}", ipstringbuffer);
				break;
			case AF_NETBIOS:
				JLOG_INFO("AF_NETBIOS (NetBIOS)");
				break;
			default:
				JLOG_INFO("Other {}", ptr->ai_family);
				break;
			}

			JLOG_INFO("\tSocket type: ");
			switch (ptr->ai_socktype) {
			case 0:
				JLOG_INFO("Unspecified");
				break;
			case SOCK_STREAM:
				JLOG_INFO("SOCK_STREAM (stream)");
				break;
			case SOCK_DGRAM:
				JLOG_INFO("SOCK_DGRAM (datagram) ");
				break;
			case SOCK_RAW:
				JLOG_INFO("SOCK_RAW (raw) ");
				break;
			case SOCK_RDM:
				JLOG_INFO("SOCK_RDM (reliable message datagram)");
				break;
			case SOCK_SEQPACKET:
				JLOG_INFO("SOCK_SEQPACKET (pseudo-stream packet)");
				break;
			default:
				JLOG_INFO("Other {}\n", ptr->ai_socktype);
				break;
			}

			JLOG_INFO("\tProtocol: ");
			switch (ptr->ai_protocol) {
			case 0:
				JLOG_INFO("Unspecified");
				break;
			case IPPROTO_TCP:
				JLOG_INFO("IPPROTO_TCP (TCP)");
				break;
			case IPPROTO_UDP:
				JLOG_INFO("IPPROTO_UDP (UDP)");
				break;
			default:
				JLOG_INFO("Other {}", ptr->ai_protocol);
				break;
			}
			JLOG_INFO("\tLength of this sockaddr: {}", ptr->ai_addrlen);
			JLOG_INFO("\tCanonical name: {}", ptr->ai_canonname ? ptr->ai_canonname : "null");

			if (ok) {
				break;
			}
		}

		freeaddrinfo(result);

		if (ok) {
			return ip;
		}
	} while (false);

	return "";
}

}
}
