/********************************************************************************************************************************
Networking Wrapper By Punal Manalan
Use this code wherever and whenever you want to!

Information About This Wrapper:
1. LightWeight, Easy to use Header-Only "NW_P.h" Wrapper for Networking(UDP/TCP).//PENDING
2. 'OCLW_P::OpenCLWrapper' Is the Entire OpenCL Program that has every information of the devices.
3. Written Using only "CL/cl.h" and has backward compatibility up to OpenCL 1.2!
4. Adding kernel function and argument types Only Once.
5. Information about each kernel function and its argument types can be Accessed with getter functions().
6. Works On Multi-Platforms and Multi-GPUs.
7. Sharing of Work-Load between Multi-GPUs Can be achieved anytime during runtime!
8. Compiles and Runs on Windows, Linux and Mac.
9. This Wrapper specializes in Heavy Computation on GPU.
10. From Construction to Destruction everything is Done automatically!
11. No need for manual destruction and releasing cl resources!
12. No need to write Tedious amount of code to counter-memory leaks!
13. Upon going out of scope or exiting the program Every resource that is used is safely Released and Deleted.
14. Details and Errors(If any) are Logged as a Log.txt file Upon Exit (NOTE: "OCLW_P.h" is tested bug-free!).
*********************************************************************************************************************************/

#pragma once

#ifndef NETWORKING_WRAPPER_BY_PUNAL
#define NETWORKING_WRAPPER_BY_PUNAL

#include "Essenbp.h"//Helper Functions and Struct By Punal

#ifdef _WIN32
#define NOMINMAX // For std::min/max is already there. This macro helps to counter it
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#else
//Linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define _SOCKCleanup
#endif

namespace NW_P//OpenCL Wrapper By Punal Manalan
{//NOTE:: WONT't WORK FOR LINUX NEED TO COMPLETE WINDOWS FIRST

	void GetAddrInfoFromHostNameIPV4(const char* DomainName, addrinfo* Result, bool& IsSuccessful)
	{
		IsSuccessful = false;
		addrinfo Hint;
		addrinfo* Return = nullptr;
		int ErrorCode;

		memset(&Hint, 0, sizeof(Hint));
		Hint.ai_family = AF_INET;
		Hint.ai_socktype = SOCK_STREAM;
		//Hint.ai_socktype = SOCK_DGRAM;

		ErrorCode = getaddrinfo(DomainName, NULL, &Hint, &Return);
		if (ErrorCode != 0)
		{			
			Essenbp::WriteLogToFile("\n Error GetAddrInfoFromHostNameIPV4() Failed with Error Code: " + std::to_string(ErrorCode) + " in GetAddrInfoFromHostName In: NW_P!");
		}
		else
		{
			*Result = *Return;
			char IpAddress[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &((sockaddr_in*)((Result)->ai_addr))->sin_addr, IpAddress, INET_ADDRSTRLEN);
			//Essenbp::WriteLogToFile("\n IPV4 Address of Domain '" + std::string(DomainName) + "' Is " + std::string(IpAddress));

			IsSuccessful = true;
		}

		if (!IsSuccessful)// For the safe of readability
		{
			Essenbp::WriteLogToFile("\n Error GetAddrInfoFromHostNameIPV4() Failed in NW_P!\n");
		}
	}

	void GetAddrInfoFromHostNameIPV6(const char* DomainName, addrinfo* Result, bool& IsSuccessful)
	{
		IsSuccessful = false;
		addrinfo Hint;
		addrinfo* Return = nullptr;
		int ErrorCode;

		memset(&Hint, 0, sizeof(Hint));
		Hint.ai_family = AF_INET6;
		Hint.ai_socktype = SOCK_STREAM;
		//Hint.ai_socktype = SOCK_DGRAM;

		ErrorCode = getaddrinfo(DomainName, NULL, &Hint, &Return);
		if (ErrorCode != 0)
		{
			Essenbp::WriteLogToFile("\n Error GetAddrInfoFromHostNameIPV6() Failed with Error Code: " + std::to_string(ErrorCode) + " in GetAddrInfoFromHostName In: NW_P!");
		}
		else
		{
			*Result = *Return;
			char IpAddress[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &((sockaddr_in6*)((Result)->ai_addr))->sin6_addr, IpAddress, INET6_ADDRSTRLEN);
			//Essenbp::WriteLogToFile("\n IPV6 Address of Domain '" + std::string(DomainName) + "' Is " + std::string(IpAddress));

			IsSuccessful = true;
		}

		if (!IsSuccessful)// For the safe of readability
		{
			Essenbp::WriteLogToFile("\n Error GetAddrInfoFromHostNameIPV6() Failed in NW_P!\n");
		}
	}

	struct NetworkWrapper
	{
		SOCKET SocketIPv4 = NULL;
		SOCKET SocketIPv6 = NULL;
		sockaddr_in ServerHintIPv4 = { 0 };//This Server or Connecting Server hint
		sockaddr_in6 ServerHintIPv6 = { 0 };//This Server or Connecting Server hint

		DWORD InputTimeOut = 0;//TCP SOCKET InputTimeOut
		DWORD OutputTimeOut = 0;//TCP SOCKET OutputTimeOut

		//For Server And Client
		NetworkWrapper(std::string IPAddress, unsigned int PortNumber, bool TrueForServerFalseForClient, bool TrueForIPv6FalseForIPv4, bool TrueForTCPFasleForUDP, bool& IsSuccessful, int SocketInputTimeoutInSeconds = 60, int SocketOutputTimeoutInSeconds = 60)
		{
			Essenbp::WriteLogToFile("\n Constructing NetworkWrapper!");

			IsSuccessful = false;

#ifdef _WIN32
			// INITIALIZE WINSOCK
			/*------------------------------------------------------------------------------------------------------------------*/
			// Structure to store the WinSock version. This is filled in
			// on the call to WSAStartup()
			WSADATA data;

			// To start WinSock, the required version must be passed to
			// WSAStartup(). This server is going to use WinSock version
			// 2 so  create a word that will store 2 and 2 in hex i.e.
			// 0x0202
			WORD version = MAKEWORD(2, 2);

			// Start WinSock
			int wsOk = WSAStartup(version, &data);
			if (wsOk != 0)
			{
				Essenbp::WriteLogToFile("\n Error Winsock Startup Failed In: NetworkWrapper!");
				Essenbp::WriteLogToFile("\n Error Construction Failed NetworkWrapper!");
				return;
			}
			/*------------------------------------------------------------------------------------------------------------------*/
#endif
			IsSuccessful = true;
			SOCKET* Socketptr = nullptr;

			if (TrueForTCPFasleForUDP)
			{
				if (TrueForIPv6FalseForIPv4)
				{
					SocketIPv6 = socket(PF_INET6, SOCK_STREAM, 0);
					Socketptr = &SocketIPv6;					
				}
				else
				{
					SocketIPv4 = socket(PF_INET, SOCK_STREAM, 0);
					Socketptr = &SocketIPv4;
				}

				if (*Socketptr == INVALID_SOCKET)
				{
					Essenbp::WriteLogToFile("\n Error socket() Failed with Error " + std::to_string(WSAGetLastError()) + " In: NetworkWrapper!");
					Socketptr = nullptr;
					IsSuccessful = false;
				}
				
				InputTimeOut = SocketInputTimeoutInSeconds * 1000;//60 Seconds InputTimeOut
				OutputTimeOut = SocketOutputTimeoutInSeconds * 1000;//60 Seconds OutputTimeOut				
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					SocketIPv6 = socket(PF_INET6, SOCK_DGRAM, 0);
					Socketptr = &SocketIPv6;
				}
				else
				{
					SocketIPv4 = socket(PF_INET, SOCK_DGRAM, 0);
					Socketptr = &SocketIPv4;
				}				

				if (*Socketptr == INVALID_SOCKET)
				{
					Essenbp::WriteLogToFile("\n Error socket() Failed with Error " + std::to_string(WSAGetLastError()) + " In: NetworkWrapper!");
					Socketptr = nullptr;
					IsSuccessful = false;
				}
			}			

			if (IsSuccessful)
			{
				if (TrueForIPv6FalseForIPv4)
				{
					memset(&ServerHintIPv4, 0, sizeof(ServerHintIPv4));
					ServerHintIPv4.sin_family = PF_INET; // Address format is IPv4
					ServerHintIPv4.sin_port = htons(PortNumber); // Convert from little to big endian
					inet_pton(PF_INET, IPAddress.c_str(), &(ServerHintIPv4.sin_addr));

					if (TrueForServerFalseForClient)
					{
						if (bind(*Socketptr, (sockaddr*)&ServerHintIPv4, sizeof(ServerHintIPv4)) == SOCKET_ERROR) // Bind The Socket to Ip And Port
						{
							Essenbp::WriteLogToFile("\n Error bind() Failed with Error " + std::to_string(WSAGetLastError()) + " In: NetworkWrapper!");
							IsSuccessful = false;
						}
					}
					else
					{
						if (connect(*Socketptr, (sockaddr*)&ServerHintIPv4, sizeof(ServerHintIPv4)) == SOCKET_ERROR) // Connect The Socket to Ip And Port
						{
							Essenbp::WriteLogToFile("\n Error connet() Failed with Error " + std::to_string(WSAGetLastError()) + " In: NetworkWrapper!");
							IsSuccessful = false;
						}
					}
				}
				else
				{
					memset(&ServerHintIPv6, 0, sizeof(ServerHintIPv6));
					ServerHintIPv6.sin6_family = PF_INET6; // Address format is IPv6
					ServerHintIPv6.sin6_port = htons(PortNumber); // Convert from little to big endian
					inet_pton(PF_INET6, IPAddress.c_str(), &(ServerHintIPv6.sin6_addr));

					if (TrueForServerFalseForClient)
					{
						if (bind(*Socketptr, (sockaddr*)&ServerHintIPv6, sizeof(ServerHintIPv6)) == SOCKET_ERROR) // Bind The Socket to Ip And Port
						{
							Essenbp::WriteLogToFile("\n Error bind() Failed with Error " + std::to_string(WSAGetLastError()) + " In: NetworkWrapper!");
							IsSuccessful = false;
						}
					}
					else
					{
						if (connect(*Socketptr, (sockaddr*)&ServerHintIPv6, sizeof(ServerHintIPv6)) == SOCKET_ERROR) // Connect The Socket to Ip And Port
						{
							Essenbp::WriteLogToFile("\n Error connet() Failed with Error " + std::to_string(WSAGetLastError()) + " In: NetworkWrapper!");
							IsSuccessful = false;
						}
					}
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetworkWrapper!");
			}
		}
	};
};


#endif // !NETWORKING_WRAPPER_BY_PUNAL