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

#include <thread>//		Multi Threading
#include <atomic>//		Threading Helper
#include "Essenbp.h"//	Helper Functions and Struct By Punal

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

#define htonll(x) ((1==htonl(1)) ? (x) : (((uint64_t)htonl((x) & 0xFFFFFFFFUL)) << 32) | htonl((uint32_t)((x) >> 32)))
#define ntohll(x) ((1==ntohl(1)) ? (x) : (((uint64_t)ntohl((x) & 0xFFFFFFFFUL)) << 32) | ntohl((uint32_t)((x) >> 32)))
#endif

namespace NW_P//OpenCL Wrapper By Punal Manalan
{//NOTE:: WONT't WORK FOR LINUX NEED TO COMPLETE WINDOWS FIRST
	bool EndianCheckDone = false;
	bool IsLittleEndian = false;

	void NW_PCheckIfLittleEndian()
	{
		if (htonl(47) != 47) 
		{
			IsLittleEndian = true;
		}
		else 
		{
			IsLittleEndian = false;
		}
		EndianCheckDone = true;
	}

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

		if (!IsSuccessful)
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

		if (!IsSuccessful)
		{
			Essenbp::WriteLogToFile("\n Error GetAddrInfoFromHostNameIPV6() Failed in NW_P!\n");
		}
	}

	void CompareIPAddr(const sockaddr_in* FirstIP, const sockaddr_in* SecondIP, bool& ReturnsTrueForSameFalseForNot)
	{
		ReturnsTrueForSameFalseForNot = ((FirstIP->sin_addr.s_addr == SecondIP->sin_addr.s_addr) && (FirstIP->sin_port == SecondIP->sin_port));
	}
	void CompareIPAddr(const sockaddr_in6* FirstIP, const sockaddr_in6* SecondIP, bool& ReturnsTrueForSameFalseForNot)
	{
		ReturnsTrueForSameFalseForNot = (FirstIP->sin6_port == SecondIP->sin6_port);
		if (ReturnsTrueForSameFalseForNot)
		{
			for (int i = 0; i < 16; ++i)
			{
				if (FirstIP->sin6_addr.s6_addr[i] != SecondIP->sin6_addr.s6_addr[i])
				{
					ReturnsTrueForSameFalseForNot = false;
					break;
				}
			}
		}
	}

	//PENDING change Variable(ArgVariable) to Variable = ArgVariable;

	//NOTE: Use NetAddr Struct Instead of NetAddrIPv4/6
	struct NetAddrIPv4
	{
	public:
		uint64_t ClientUniqueID = 0;//PENDING This should be changed Every Specified Minutes For Security Purposes...

		SOCKET Socket;
		uint64_t UniqueNumber;//Number = 0 => Server, Number > 0 => Client //PENDING Modify this with functions
		sockaddr_in NetAddress;
		const uint16_t SentPacketsArchiveSize;
		const uint16_t ReceivedPacketsArchiveSize;

		Essenbp::ArrayOfUnknownDataAndSize SentPackets;
		Essenbp::ArrayOfUnknownDataAndSize ReceivedPackets;
		uint16_t SentCount = 0;//		NOTE: Counter Resets to 0 When Max SentPacketsArchiveSize		Is Reached, Previous Data at 0 and so on will be Overwritten
		uint16_t ReceivedCount = 0;//	NOTE: Counter Resets to 0 When Max ReceivedPacketsArchiveSize	Is Reached, Previous Data at 0 and so on will be Overwritten

		bool IsConstructionSuccessful = false;

		//For UDP set ArgClientSocket == NULL
		NetAddrIPv4(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize) : Socket(ArgSocket), UniqueNumber(ArgUniqueNumber), NetAddress(ArgNetAddress), SentPacketsArchiveSize(ArgSentPacketsArchiveSize), ReceivedPacketsArchiveSize(ArgSentPacketsArchiveSize)
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddrIPv4!");

			ClientUniqueID = (uint64_t)&ArgSocket;//NOTE: using This Instead Of rand / random  //PENDING

			SentPackets.ResizeArray(SentPacketsArchiveSize, IsConstructionSuccessful);
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::ResizeArray for SentPackets Failed NetAddrIPv4!");
			}
			else
			{
				ReceivedPackets.ResizeArray(ReceivedPacketsArchiveSize, IsConstructionSuccessful);
				if (!IsConstructionSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::ResizeArray for ReceivedPackets Failed NetAddrIPv4!");
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddrIPv4!");
			}
		}

		void AddSentPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddSentPackage Without Constructing the struct In: NetAddrIPv4!\n");
			}
			else
			{
				Essenbp::UnknownDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				SentPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::GetData Failed in AddSentPackage In: NetAddrIPv4!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Essenbp::UnknownDataAndSizeStruct::CopyAndStoreData Failed in AddSentPackage In: NetAddrIPv4!");
					}
					else
					{
						SentCount = SentCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddReceivedPackage() Failed In: NetAddrIPv4!");
			}
		}

		void AddReceivedPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddReceivedPackage Without Constructing the struct In: NetAddrIPv4!\n");
			}
			else
			{
				Essenbp::UnknownDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				ReceivedPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::GetData Failed in AddReceivedPackage In: NetAddrIPv4!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Essenbp::UnknownDataAndSizeStruct::CopyAndStoreData Failed in AddReceivedPackage In: NetAddrIPv4!");
					}
					else
					{
						ReceivedCount = ReceivedCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddSentPackage() Failed In: NetAddrIPv4!");
			}
		}

		void GetSentPackage(uint16_t PacketNumber, Essenbp::UnknownDataAndSizeStruct** ReturnData, bool& IsSuccessful)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetSentPackage Without Constructing the struct In: NetAddrIPv4!\n");
			}
			else
			{
				SentPackets.GetData(PacketNumber, ReturnData, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error SentPackets.GetData(" + std::to_string(PacketNumber) + ") Failed in GetSentPackage In : NetAddrIPv4!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error GetSentPackage() Failed In: NetAddrIPv4!");
			}
		}

		void GetReceivedPackage(uint16_t PacketNumber, Essenbp::UnknownDataAndSizeStruct** ReturnData, bool& IsSuccessful)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetReceivedPackage Without Constructing the struct In: NetAddrIPv4!\n");
			}
			else
			{
				ReceivedPackets.GetData(PacketNumber, ReturnData, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ReceivedPackets.GetData(" + std::to_string(PacketNumber) + ") Failed in GetReceivedPackage In : NetAddrIPv4!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error GetReceivedPackage() Failed In: NetAddrIPv4!");
			}
		}

		uint64_t GetClientUniqueID()
		{
			return ClientUniqueID;
		}

		uint64_t GetUniqueNumber()
		{
			return UniqueNumber;
		}

		~NetAddrIPv4()
		{
			Essenbp::WriteLogToFile("\n Destructing NetAddrIPv4!");
			if (IsConstructionSuccessful)
			{
				if (Socket != NULL)
				{
					closesocket(Socket);
				}
				IsConstructionSuccessful = false;
			}
		}
	};

	//NOTE: Use NetAddr Struct Instead of NetAddrIPv4/6
	struct NetAddrIPv6
	{
	public:
		uint64_t ClientUniqueID = 0;//This should be changed Every Specified Minutes For Security Purposes...

		SOCKET Socket;
		uint64_t UniqueNumber;//Number = 0 => Server, Number > 0 => Client //PENDING Modify this with functions
		sockaddr_in6 NetAddress;
		const uint16_t SentPacketsArchiveSize;
		const uint16_t ReceivedPacketsArchiveSize;

		Essenbp::ArrayOfUnknownDataAndSize SentPackets;
		Essenbp::ArrayOfUnknownDataAndSize ReceivedPackets;
		uint16_t SentCount = 0;//		NOTE: Counter Resets to 0 When Max SentPacketsArchiveSize		Is Reached, Previous Data at 0 and so on will be Overwritten
		uint16_t ReceivedCount = 0;//	NOTE: Counter Resets to 0 When Max ReceivedPacketsArchiveSize	Is Reached, Previous Data at 0 and so on will be Overwritten

		bool IsConstructionSuccessful = false;

		//For UDP set ArgClientSocket == NULL
		NetAddrIPv6(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in6 ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize) : Socket(ArgSocket), UniqueNumber(ArgUniqueNumber), NetAddress(ArgNetAddress), SentPacketsArchiveSize(ArgSentPacketsArchiveSize), ReceivedPacketsArchiveSize(ArgSentPacketsArchiveSize)
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddrIPv6!");

			ClientUniqueID = (uint64_t)&ArgSocket;//NOTE: using This Instead Of rand / random  //PENDING

			SentPackets.ResizeArray(SentPacketsArchiveSize, IsConstructionSuccessful);
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::ResizeArray for SentPackets Failed NetAddrIPv6!");
			}
			else
			{
				ReceivedPackets.ResizeArray(ReceivedPacketsArchiveSize, IsConstructionSuccessful);
				if (!IsConstructionSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::ResizeArray for ReceivedPackets Failed NetAddrIPv6!");
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddrIPv6!");
			}
		}

		//PENDING sent And Received Count Reset not made yet....
		void AddSentPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddSentPackage Without Constructing the struct In: NetAddrIPv6!\n");
			}
			else
			{
				Essenbp::UnknownDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				SentPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::GetData Failed in AddSentPackage In: NetAddrIPv6!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Essenbp::UnknownDataAndSizeStruct::CopyAndStoreData Failed in AddSentPackage In: NetAddrIPv6!");
					}
					else
					{
						SentCount = SentCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddSentPackage() Failed In: NetAddrIPv6!");
			}
		}

		void AddReceivedPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddReceivedPackage Without Constructing the struct In: NetAddrIPv6!\n");
			}
			else
			{
				Essenbp::UnknownDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				ReceivedPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ArrayOfUnknownDataAndSize::GetData Failed in AddReceivedPackage In: NetAddrIPv6!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Essenbp::UnknownDataAndSizeStruct::CopyAndStoreData Failed in AddReceivedPackage In: NetAddrIPv6!");
					}
					else
					{
						ReceivedCount = ReceivedCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddReceivedPackage() Failed In: NetAddrIPv6!");
			}
		}

		void GetSentPackage(uint16_t PacketNumber, Essenbp::UnknownDataAndSizeStruct** ReturnData, bool& IsSuccessful)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetSentPackage Without Constructing the struct In: NetAddrIPv6!\n");
			}
			else
			{
				SentPackets.GetData(PacketNumber, ReturnData, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error SentPackets.GetData(" + std::to_string(PacketNumber) + ") Failed in GetSentPackage In : NetAddrIPv6!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error GetSentPackage() Failed In: NetAddrIPv6!");
			}
		}

		void GetReceivedPackage(uint16_t PacketNumber, Essenbp::UnknownDataAndSizeStruct** ReturnData, bool& IsSuccessful)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetReceivedPackage Without Constructing the struct In: NetAddrIPv6!\n");
			}
			else
			{
				ReceivedPackets.GetData(PacketNumber, ReturnData, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ReceivedPackets.GetData(" + std::to_string(PacketNumber) + ") Failed in GetReceivedPackage In : NetAddrIPv6!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error GetReceivedPackage() Failed In: NetAddrIPv6!");
			}
		}

		uint64_t GetClientUniqueID()
		{
			return ClientUniqueID;
		}

		uint64_t GetUniqueNumber()
		{
			return UniqueNumber;
		}

		~NetAddrIPv6()
		{
			Essenbp::WriteLogToFile("\n Destructing NetAddrIPv6!");
			if (IsConstructionSuccessful)
			{
				if (Socket != NULL)
				{
					closesocket(Socket);
				}
				IsConstructionSuccessful = false;
			}
		}
	};

	//NOTE: For Both IPv4 And IPv6
	//NOTE: This Version will have its CustomStructOrClassptr Data Safely Deleted but two new variables are added to achive that(Basically Size is increased)
	struct Sole_NetAddr
	{
	public:
		bool TrueForIPv6FalseForIPv4;
		void* IPAddr;
		void* CustomStructOrClassptr = nullptr;
		//void (*CustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful) = nullptr;//Not Needed 
		void (*CustomStructOrClass_Destructorfunctionptr)(void** PtrToCustomStructOrClassptr) = nullptr;

		bool IsConstructionSuccessful;

		Sole_NetAddr(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize, void (*ArgCustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful), void (*ArgCustomStructOrClass_Destructorfunctionptr)(void** PtrToCustomStructOrClassptr)) : TrueForIPv6FalseForIPv4(false), IPAddr(new NetAddrIPv4(ArgSocket, ArgUniqueNumber, ArgNetAddress, ArgSentPacketsArchiveSize, ArgReceivedPacketsArchiveSize))
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddr!");

			IsConstructionSuccessful = false;

			CustomStructOrClassptr = nullptr;

			if (IPAddr != nullptr)
			{
				if (((NetAddrIPv4*)IPAddr)->IsConstructionSuccessful)
				{
					if (ArgCustomStructOrClass_Constructorfunctionptr == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Constructorfunctionptr Is Nullptr In: NetAddr!\n");
					}
					else
					{
						if (ArgCustomStructOrClass_Destructorfunctionptr == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Destructorfunctionptr Is Nullptr In: NetAddr!\n");
						}
						else
						{
							ArgCustomStructOrClass_Constructorfunctionptr(&CustomStructOrClassptr, IsConstructionSuccessful);
							if (IsConstructionSuccessful)
							{
								CustomStructOrClass_Destructorfunctionptr = ArgCustomStructOrClass_Destructorfunctionptr;
							}
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error IPAddr Constrction Failed In: NetAddr!\n");
					delete IPAddr;
				}
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(NetAddrIPv4)) + " Byes Of Memory for IPAddr In: NetAddr!\n");
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddr!");
			}
		}

		Sole_NetAddr(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in6 ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize, void (*ArgCustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful), void (*ArgCustomStructOrClass_Destructorfunctionptr)(void** PtrToCustomStructOrClassptr)) : TrueForIPv6FalseForIPv4(true), IPAddr(new NetAddrIPv6(ArgSocket, ArgUniqueNumber, ArgNetAddress, ArgSentPacketsArchiveSize, ArgReceivedPacketsArchiveSize))
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddr!");

			IsConstructionSuccessful = false;

			CustomStructOrClassptr = nullptr;

			if (IPAddr != nullptr)
			{
				if (((NetAddrIPv6*)IPAddr)->IsConstructionSuccessful)
				{
					if (ArgCustomStructOrClass_Constructorfunctionptr == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Constructorfunctionptr Is Nullptr In: NetAddr!\n");
					}
					else
					{
						if (ArgCustomStructOrClass_Destructorfunctionptr == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Destructorfunctionptr Is Nullptr In: NetAddr!\n");
						}
						else
						{
							ArgCustomStructOrClass_Constructorfunctionptr(&CustomStructOrClassptr, IsConstructionSuccessful);
							if (IsConstructionSuccessful)
							{
								CustomStructOrClass_Destructorfunctionptr = ArgCustomStructOrClass_Destructorfunctionptr;
							}
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error IPAddr Constrction Failed In: NetAddr!\n");
					delete IPAddr;
				}
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(NetAddrIPv4)) + " Byes Of Memory for IPAddr In: NetAddr!\n");
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddr!");
			}
		}

		void AddSentPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddSentPackage Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					((NetAddrIPv6*)IPAddr)->AddSentPackage(Data, DataSize, IsSuccessful);
				}
				else
				{
					((NetAddrIPv4*)IPAddr)->AddSentPackage(Data, DataSize, IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error AddSentPackage() Failed In: NetAddr!");
				}
			}
		}

		void AddReceivedPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddReceivedPackage Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					((NetAddrIPv6*)IPAddr)->AddReceivedPackage(Data, DataSize, IsSuccessful);
				}
				else
				{
					((NetAddrIPv4*)IPAddr)->AddReceivedPackage(Data, DataSize, IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error AddReceivedPackage() Failed In: NetAddr!");
				}
			}
		}

		uint64_t GetClientUniqueID()
		{
			if (TrueForIPv6FalseForIPv4)
			{
				return ((NetAddrIPv6*)IPAddr)->GetClientUniqueID();
			}
			else
			{
				return ((NetAddrIPv4*)IPAddr)->GetClientUniqueID();
			}
		}

		uint64_t GetUniqueNumber()
		{
			if (TrueForIPv6FalseForIPv4)
			{
				return ((NetAddrIPv6*)IPAddr)->GetUniqueNumber();
			}
			else
			{
				return ((NetAddrIPv4*)IPAddr)->GetUniqueNumber();
			}
		}

		~Sole_NetAddr()
		{
			Essenbp::WriteLogToFile("\n Destructing NetAddr!");
			if (IsConstructionSuccessful)
			{				
				if (IPAddr != nullptr)
				{
					delete IPAddr;
					IPAddr = nullptr;
				}
				if (CustomStructOrClassptr != nullptr)
				{
					CustomStructOrClass_Destructorfunctionptr(&CustomStructOrClassptr);
					if (IsConstructionSuccessful)
					{
						CustomStructOrClassptr = nullptr;
					}
					IsConstructionSuccessful = !IsConstructionSuccessful;
				}
				IsConstructionSuccessful = false;
			}
		}
	};

	//NOTE: For Both IPv4 And IPv6
	//NOTE: This is meant for NetAddrArray Which Takes care of its construction and destruction
	//NOTE: For this Version The CustomStructOrClassptr Data is to be Destroyed Externally, The Plus point of this is that this has less size...
	//NOTE: In Other Words ALWAYS REMEMBER TO SAFELY DELETE THIS(But anyway NO Manual destruction is required since This NetAddrArray will take care of it)
	//NOTE: For UDP Netaddr set the Socket to NULL
	struct NetAddr
	{
	public:
		bool TrueForIPv6FalseForIPv4;
		void* IPAddr;
		void* CustomStructOrClassptr = nullptr;
		uint64_t LastGetAddTime = 0;// In Seconds

		bool IsConstructionSuccessful;

		//NOTE: For UDP Netaddr set the ArgSocket to NULL
		NetAddr(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize, void (*ArgCustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful)) : TrueForIPv6FalseForIPv4(false), IPAddr(new NetAddrIPv4(ArgSocket, ArgUniqueNumber, ArgNetAddress, ArgSentPacketsArchiveSize, ArgReceivedPacketsArchiveSize))
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddr!");

			IsConstructionSuccessful = false;

			CustomStructOrClassptr = nullptr;
			LastGetAddTime = 0;

			if (IPAddr != nullptr)
			{
				if (((NetAddrIPv4*)IPAddr)->IsConstructionSuccessful)
				{
					if (ArgCustomStructOrClass_Constructorfunctionptr == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Constructorfunctionptr Is Nullptr In: NetAddr!\n");
					}
					else
					{
						ArgCustomStructOrClass_Constructorfunctionptr(&CustomStructOrClassptr, IsConstructionSuccessful);
						if (!IsConstructionSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error CustomStructOrClassptr Constrction Failed In: NetAddr!\n");
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error IPAddr Constrction Failed In: NetAddr!\n");
					delete IPAddr;
				}
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(NetAddrIPv4)) + " Byes Of Memory for IPAddr In: NetAddr!\n");
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddr!");
			}
		}

		//NOTE: For UDP Netaddr set the ArgSocket to NULL
		NetAddr(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in6 ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize, void (*ArgCustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful)) : TrueForIPv6FalseForIPv4(true), IPAddr(new NetAddrIPv6(ArgSocket, ArgUniqueNumber, ArgNetAddress, ArgSentPacketsArchiveSize, ArgReceivedPacketsArchiveSize))
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddr!");

			IsConstructionSuccessful = false;

			CustomStructOrClassptr = nullptr;
			LastGetAddTime = 0;

			if (IPAddr != nullptr)
			{
				if (((NetAddrIPv4*)IPAddr)->IsConstructionSuccessful)
				{
					if (ArgCustomStructOrClass_Constructorfunctionptr == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Constructorfunctionptr Is Nullptr In: NetAddr!\n");
					}
					else
					{
						ArgCustomStructOrClass_Constructorfunctionptr(&CustomStructOrClassptr, IsConstructionSuccessful);
						if (!IsConstructionSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error CustomStructOrClassptr Constrction Failed In: NetAddr!\n");
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error IPAddr Constrction Failed In: NetAddr!\n");
					delete IPAddr;
				}
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(NetAddrIPv4)) + " Byes Of Memory for IPAddr In: NetAddr!\n");
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddr!");
			}
		}

		void AddSentPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddSentPackage Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					((NetAddrIPv6*)IPAddr)->AddSentPackage(Data, DataSize, IsSuccessful);
				}
				else
				{
					((NetAddrIPv4*)IPAddr)->AddSentPackage(Data, DataSize, IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error AddSentPackage() Failed In: NetAddr!");
				}
				else
				{
					LastGetAddTime =  Essenbp::TimeSinceEpochInSecond();
				}
			}
		}

		void AddReceivedPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddReceivedPackage Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					((NetAddrIPv6*)IPAddr)->AddReceivedPackage(Data, DataSize, IsSuccessful);
				}
				else
				{
					((NetAddrIPv4*)IPAddr)->AddReceivedPackage(Data, DataSize, IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error AddReceivedPackage() Failed In: NetAddr!");
				}
				else
				{
					LastGetAddTime = Essenbp::TimeSinceEpochInSecond();
				}
			}
		}

		void GetSentPackage(uint16_t PacketNumber, Essenbp::UnknownDataAndSizeStruct** ReturnData, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetSentPackage Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					((NetAddrIPv6*)IPAddr)->GetSentPackage(PacketNumber, ReturnData, IsSuccessful);
				}
				else
				{
					((NetAddrIPv4*)IPAddr)->GetSentPackage(PacketNumber, ReturnData, IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error GetSentPackage() Failed In: NetAddr!");
				}
				else
				{
					LastGetAddTime = Essenbp::TimeSinceEpochInSecond();
				}
			}
		}

		void GetReceivedPackage(uint16_t PacketNumber, Essenbp::UnknownDataAndSizeStruct** ReturnData, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetReceivedPackage Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					((NetAddrIPv6*)IPAddr)->GetReceivedPackage(PacketNumber, ReturnData, IsSuccessful);
				}
				else
				{
					((NetAddrIPv4*)IPAddr)->GetReceivedPackage(PacketNumber, ReturnData, IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error GetReceivedPackage() Failed In: NetAddr!");
				}
				else
				{
					LastGetAddTime = Essenbp::TimeSinceEpochInSecond();
				}
			}
		}

		uint64_t GetClientUniqueID()
		{
			if (TrueForIPv6FalseForIPv4)
			{
				return ((NetAddrIPv6*)IPAddr)->GetClientUniqueID();
			}
			else
			{
				return ((NetAddrIPv4*)IPAddr)->GetClientUniqueID();
			}
		}

		uint64_t GetUniqueNumber()
		{
			if (TrueForIPv6FalseForIPv4)
			{
				return ((NetAddrIPv6*)IPAddr)->GetUniqueNumber();
			}
			else
			{
				return ((NetAddrIPv4*)IPAddr)->GetUniqueNumber();
			}
		}

		SOCKET GetSocket()
		{
			if (TrueForIPv6FalseForIPv4)
			{
				return ((NetAddrIPv6*)IPAddr)->Socket;
			}
			else
			{
				return ((NetAddrIPv4*)IPAddr)->Socket;
			}
		}

		void FreeOrDeleteCustomStructOrClassptr(void (*CustomStructOrClass_Destructorfunctionptr)(void** PtrToCustomStructOrClassptr))
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling FreeOrDeleteCustomStructOrClassptr Without Constructing the struct In: NetAddr!\n");
			}
			else
			{
				CustomStructOrClass_Destructorfunctionptr(&CustomStructOrClassptr);
			}
		}

		~NetAddr()
		{
			Essenbp::WriteLogToFile("\n Destructing NetAddr!");
			if (IsConstructionSuccessful)
			{
				if (IPAddr != nullptr)
				{
					delete IPAddr;
					IPAddr = nullptr;
				}
				if (CustomStructOrClassptr != nullptr)
				{
					Essenbp::WriteLogToFile("\n Error CustomStructOrClassptr was not Deleted/Freed In: NetAddr!\n");
				}
				IsConstructionSuccessful = false;
			}
		}
	};

	//NOTE: A List of NetAddar(With const values for Construction)
	//NOTE: MinumimFreeSpotsInArray Means At a time how many free spots can be Reserved, It is reserved each time when the current reserve is filled
	//NOTE: MaxUnderflowedFreeSpotsInArray Is always Higher than Minimum
	//NOTE: MaxUnderflowedFreeSpotsInArray Defines how many Active Spots can be Be Removed, when the total number of removed is equal to MaxUnderflowedFreeSpotsInArray, the Array is reordered
	//NOTE: TotalNumberOfNetAddr Maximum Value is A Multiple of MaxUnderflowedFreeSpotsInArray	
	//NOTE: Array Number of 0 Is Reserved, So Array Number Starts from 1 to TotalNumberOfNetAddr(== (n * MaxUnderflowedFreeSpotsInArray) + 1)
	//NOTE: Lets say Initially TotalNumberOfNetAddr = (1 * MaxUnderflowedFreeSpotsInArray) + 1 Once every Spots is Used
	//NOTE: It will again be Filled with more free SpotsTotalNumberOfNetAddr = (2 * MaxUnderflowedFreeSpotsInArray) + 1 and So on until buffer overflow...
	//NOTE: CustomStructOrClass_Constructorfunctionptr is used to construct(or store) any type of struct, class or any data for every Netaddr(Program Code for each connected client or anyother purpose)
	//NOTE: CustomStructOrClass_Destructorfunctionptr is used to destruct(or free) the struct, class or data that is constructed by CustomStructOrClass_Constructorfunctionptr
	struct NetAddrArray
	{
	private:
		NetAddr** ArrayOfNetAddr = nullptr;
		uint64_t TotalNumberOfNetAddr = 0;
		void (*CustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful) = nullptr;
		void (*CustomStructOrClass_Destructorfunctionptr)(void** PtrToCustomStructOrClassptr) = nullptr;		

		const uint64_t MaxReservedFreeSpotsInArray = 0;//If Every Reserved Spot the Used then new Reserved Spots with size MaxReservedFreeSpotsInArray is added the Array Is Reordered Only when Increasing
		const uint64_t MaxUnderflowedFreeSpotsInArray = 0;//Reordered Only when Decreasing
		const uint16_t SentPacketsArchiveSize;
		const uint16_t ReceivedPacketsArchiveSize;

		uint64_t* ReservedNetAddarSpotsInArray = nullptr;
		uint64_t RemainingNumberOfReservedNetAddarSpotsInArray = 0;//RemainingNumberOfReservedNetAddarSpotsInArray = MaxReservedFreeSpotsInArray//When Creating
		uint64_t* UnderflowedNetAddarSpotsInArray = nullptr;
		uint64_t RemainingNumberOfUnderflowedNetAddarSpotsInArray = 0;//RemainingNumberOfUnderflowedNetAddarSpotsInArray = 0// when No element is removed from active spot,

		uint64_t TotalNumberOfReorderedArray = 0;//NOTE: n = MaxReservedFreeSpotsInArray + MaxUnderflowedFreeSpotsInArray
		uint64_t* ReorderedArrayNumbers = nullptr;//NOTE: Total Size = n*2*(sizeof(uint64_t)) //[(k*2)] is Previous Array Number, [(k*2) + 1] is the Current(Reordered) Array Number
		bool IsConstructionSuccessful = false;


	public:
		NetAddrArray(uint64_t ArgMaxReservedFreeSpotsInArray, uint64_t ArgMaxUnderflowedFreeSpotsInArray, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize, void (*ArgCustomStructOrClass_Constructorfunctionptr)(void** PtrToCustomStructOrClassptr, bool& IsSuccessful), void (*ArgCustomStructOrClass_Destructorfunctionptr)(void** PtrToCustomStructOrClassptr), NetAddr* BaseNetAddr = nullptr) : MaxReservedFreeSpotsInArray(ArgMaxReservedFreeSpotsInArray), MaxUnderflowedFreeSpotsInArray(ArgMaxUnderflowedFreeSpotsInArray), SentPacketsArchiveSize(ArgSentPacketsArchiveSize), ReceivedPacketsArchiveSize(ArgSentPacketsArchiveSize)
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddrArray!");

			IsConstructionSuccessful = false;

			ArrayOfNetAddr = nullptr;
			TotalNumberOfNetAddr = 0;

			ReservedNetAddarSpotsInArray = nullptr;
			RemainingNumberOfReservedNetAddarSpotsInArray = 0;
			UnderflowedNetAddarSpotsInArray = nullptr;
			RemainingNumberOfUnderflowedNetAddarSpotsInArray = 0;
			TotalNumberOfReorderedArray = 0;
			ReorderedArrayNumbers = nullptr;

			if (MaxReservedFreeSpotsInArray == 0)
			{
				Essenbp::WriteLogToFile("\n Error MaxReservedFreeSpotsInArray is Equal to 0 In: NetAddrArray!\n");
			}
			else
			{
				if (MaxUnderflowedFreeSpotsInArray == 0)
				{
					Essenbp::WriteLogToFile("\n Error MaxUnderflowedFreeSpotsInArray is Equal to 0 In: NetAddrArray!\n");
				}
				else
				{
					if (ArgCustomStructOrClass_Constructorfunctionptr == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Constructorfunctionptr is nullptr In: NetAddrArray!\n");
					}
					else
					{
						if (ArgCustomStructOrClass_Destructorfunctionptr == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error ArgCustomStructOrClass_Destructorfunctionptr is nullptr In: NetAddrArray!\n");
						}
						else
						{
							ReservedNetAddarSpotsInArray = (uint64_t*)calloc((MaxReservedFreeSpotsInArray), sizeof(uint64_t));
							if (ReservedNetAddarSpotsInArray == nullptr)
							{
								Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((MaxReservedFreeSpotsInArray) * sizeof(uint64_t)) + " Byes Of Memory for ReservedNetAddarSpotsInArray In: NetAddrArray!\n");
							}
							else
							{
								UnderflowedNetAddarSpotsInArray = (uint64_t*)calloc((MaxUnderflowedFreeSpotsInArray), sizeof(uint64_t));
								if (UnderflowedNetAddarSpotsInArray == nullptr)
								{
									free(ReservedNetAddarSpotsInArray);
									Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((MaxUnderflowedFreeSpotsInArray) * sizeof(uint64_t)) + " Byes Of Memory for UnderflowedNetAddarSpotsInArray In: NetAddrArray!\n");
								}
								else
								{
									ReorderedArrayNumbers = (uint64_t*)calloc(((MaxReservedFreeSpotsInArray + MaxUnderflowedFreeSpotsInArray) * 2), sizeof(uint64_t));
									if (ReorderedArrayNumbers == nullptr)
									{
										free(UnderflowedNetAddarSpotsInArray);
										free(ReservedNetAddarSpotsInArray);
										Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(((MaxReservedFreeSpotsInArray + MaxUnderflowedFreeSpotsInArray) * 2) * sizeof(uint64_t)) + " Byes Of Memory for ReorderedArrayNumbers In: NetAddrArray!\n");
									}
									else
									{
										Essenbp::Malloc_PointerToArrayOfPointers((void***)&ArrayOfNetAddr, (MaxReservedFreeSpotsInArray + 1), sizeof(NetAddr*), IsConstructionSuccessful);
										if (!IsConstructionSuccessful)
										{
											free(UnderflowedNetAddarSpotsInArray);
											free(ReservedNetAddarSpotsInArray);
											free(ReorderedArrayNumbers);
											Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((MaxReservedFreeSpotsInArray + 1) * sizeof(NetAddr*)) + " Byes Of Memory for ArrayOfNetAddr In: NetAddrArray!\n");
										}
										else
										{
											CustomStructOrClass_Constructorfunctionptr = ArgCustomStructOrClass_Constructorfunctionptr;
											CustomStructOrClass_Destructorfunctionptr = ArgCustomStructOrClass_Destructorfunctionptr;

											for (uint64_t i = 0; i < MaxReservedFreeSpotsInArray; ++i)
											{
												ArrayOfNetAddr[i] = nullptr;
												ReservedNetAddarSpotsInArray[i] = MaxReservedFreeSpotsInArray - i;
												ReorderedArrayNumbers[(i * 2)] = 0;
												ReorderedArrayNumbers[((i * 2) + 1)] = 0;
											}
											ArrayOfNetAddr[0] = BaseNetAddr;

											for (uint64_t i = 0; i < MaxUnderflowedFreeSpotsInArray; ++i)
											{
												UnderflowedNetAddarSpotsInArray[i] = 0;
												ReorderedArrayNumbers[((i * 2) + MaxReservedFreeSpotsInArray)] = 0;
												ReorderedArrayNumbers[((i * 2) + 1 + MaxReservedFreeSpotsInArray)] = 0;
											}
											RemainingNumberOfReservedNetAddarSpotsInArray = MaxReservedFreeSpotsInArray;
											RemainingNumberOfUnderflowedNetAddarSpotsInArray = 0;
											TotalNumberOfReorderedArray = 0;
										}
									}
								}
							}
						}
					}
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddrArray!");
			}
		}

		//PENDING SET RETURN VALUE FOR THIS VERY IMPORTANT
		//NOTE: For UDP Netaddr set the Socket to NULL
		uint64_t AddNetAddr(SOCKET Socket, sockaddr_in Address, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddNetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (TotalNumberOfReorderedArray > 0)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Add Element but the previous Reordering of array is not resolved In AddNetAddr In: NetAddrArray!\n");
				}
				else
				{
					//NOTE: The Spot Array Number 0 Is Reserved
					if (RemainingNumberOfUnderflowedNetAddarSpotsInArray > 0)
					{
						ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]] = new NetAddr(Socket, UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)], Address, SentPacketsArchiveSize, ReceivedPacketsArchiveSize, CustomStructOrClass_Constructorfunctionptr);// NO need for IsSuccessful Constrction check as it is not needed in this simple struct
						if (ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]] != nullptr)
						{
							if (ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]]->IsConstructionSuccessful == false)
							{
								IsSuccessful = false;
								delete ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]];
								ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]] = nullptr;
							}
							else
							{
								UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)] = 0;//Reseted
								RemainingNumberOfUnderflowedNetAddarSpotsInArray = RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1;
								TotalNumberOfNetAddr = TotalNumberOfNetAddr + 1;

								IsSuccessful = true;
							}
						}
					}
					else
					{
						if (RemainingNumberOfReservedNetAddarSpotsInArray > 0)
						{
							//ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] == Fills the First Unused Element
							ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] = new NetAddr(Socket, ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)], Address, SentPacketsArchiveSize, ReceivedPacketsArchiveSize, CustomStructOrClass_Constructorfunctionptr);// NO need for IsSuccessful Constrction check as it is not needed in this simple struct
							if (ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] != nullptr)
							{
								if (ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]]->IsConstructionSuccessful == false)
								{
									IsSuccessful = false;
									delete ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]];
									ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] = nullptr;
								}
								else
								{
									ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)] = 0;//Reseted
									RemainingNumberOfReservedNetAddarSpotsInArray = RemainingNumberOfReservedNetAddarSpotsInArray - 1;
									TotalNumberOfNetAddr = TotalNumberOfNetAddr + 1;

									IsSuccessful = true;
								}
							}
						}
						else
						{
							NetAddr** TEMPArrayOfNetAddr = nullptr;
							Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPArrayOfNetAddr, (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray + 1), sizeof(NetAddr*), IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray + 1) * sizeof(NetAddr*)) + " Byes Of Memory for TEMPArrayOfNetAddr in AddNetAddr In: NetAddrArray!\n");
							}
							else
							{
								//This Copies Previous
								uint64_t i = 0;
								for (i = 0; i < TotalNumberOfNetAddr + 1; ++i)//NOTE: Array Starts from 0 to n Normally but, 0 is reserved so , 1 to n + 1 length
								{
									TEMPArrayOfNetAddr[i] = ArrayOfNetAddr[i];
								}

								//Sets nullptr for Allocated Space
								for (i = TotalNumberOfNetAddr + 1; i < (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray); ++i)
								{
									TEMPArrayOfNetAddr[i] = nullptr;
								}

								for (i = 0; i < MaxReservedFreeSpotsInArray; ++i)
								{
									ReservedNetAddarSpotsInArray[i] = (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray) - i;
								}

								RemainingNumberOfReservedNetAddarSpotsInArray = MaxReservedFreeSpotsInArray;
								free(ArrayOfNetAddr);
								ArrayOfNetAddr = TEMPArrayOfNetAddr;

								IsSuccessful = true;
							}
							if (!IsSuccessful)
							{
								AddNetAddr(Socket, Address, IsSuccessful);//PENDING Check this Function Upon Testing
							}
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddNetAddr() Failed in NetAddrArray!");
			}
		}

		//NOTE: For UDP Netaddr set the Socket to NULL
		uint64_t AddNetAddr(SOCKET Socket, sockaddr_in6 Address, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddNetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (TotalNumberOfReorderedArray > 0)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Add Element but the previous Reordering of array is not resolved In AddNetAddr In: NetAddrArray!\n");
				}
				else
				{
					//NOTE: The Spot Array Number 0 Is Reserved
					if (RemainingNumberOfUnderflowedNetAddarSpotsInArray > 0)
					{
						ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]] = new NetAddr(Socket, UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)], Address, SentPacketsArchiveSize, ReceivedPacketsArchiveSize, CustomStructOrClass_Constructorfunctionptr);// NO need for IsSuccessful Constrction check as it is not needed in this simple struct
						if (ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]] != nullptr)
						{
							if (ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]]->IsConstructionSuccessful == false)
							{
								IsSuccessful = false;
								delete ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]];
								ArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)]] = nullptr;
							}
							else
							{
								UnderflowedNetAddarSpotsInArray[(RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1)] = 0;//Reseted
								RemainingNumberOfUnderflowedNetAddarSpotsInArray = RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1;
								TotalNumberOfNetAddr = TotalNumberOfNetAddr + 1;

								IsSuccessful = true;
							}
						}
					}
					else
					{
						if (RemainingNumberOfReservedNetAddarSpotsInArray > 0)
						{
							//ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] == Fills the First Unused Element
							ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] = new NetAddr(Socket, ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)], Address, SentPacketsArchiveSize, ReceivedPacketsArchiveSize, CustomStructOrClass_Constructorfunctionptr);// NO need for IsSuccessful Constrction check as it is not needed in this simple struct
							if (ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] != nullptr)
							{
								if (ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]]->IsConstructionSuccessful == false)
								{
									IsSuccessful = false;
									delete ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]];
									ArrayOfNetAddr[ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)]] = nullptr;
								}
								else
								{
									ReservedNetAddarSpotsInArray[(RemainingNumberOfReservedNetAddarSpotsInArray - 1)] = 0;//Reseted
									RemainingNumberOfReservedNetAddarSpotsInArray = RemainingNumberOfReservedNetAddarSpotsInArray - 1;
									TotalNumberOfNetAddr = TotalNumberOfNetAddr + 1;

									IsSuccessful = true;
								}
							}
						}
						else
						{
							NetAddr** TEMPArrayOfNetAddr = nullptr;
							Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPArrayOfNetAddr, (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray + 1), sizeof(NetAddr*), IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray + 1) * sizeof(NetAddr*)) + " Byes Of Memory for TEMPArrayOfNetAddr in AddNetAddr In: NetAddrArray!\n");
							}
							else
							{
								//This Copies Previous
								uint64_t i = 0;
								for (i = 0; i < TotalNumberOfNetAddr + 1; ++i)//NOTE: Array Starts from 0 to n Normally but, 0 is reserved so , 1 to n + 1 length
								{
									TEMPArrayOfNetAddr[i] = ArrayOfNetAddr[i];
								}

								//Sets nullptr for Allocated Space
								for (i = TotalNumberOfNetAddr + 1; i < (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray); ++i)
								{
									TEMPArrayOfNetAddr[i] = nullptr;
								}

								for (i = 0; i < MaxReservedFreeSpotsInArray; ++i)
								{
									ReservedNetAddarSpotsInArray[i] = (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray) - i;
								}

								RemainingNumberOfReservedNetAddarSpotsInArray = MaxReservedFreeSpotsInArray;
								free(ArrayOfNetAddr);
								ArrayOfNetAddr = TEMPArrayOfNetAddr;

								IsSuccessful = true;
							}
							if (!IsSuccessful)
							{
								AddNetAddr(Socket, Address, IsSuccessful);//PENDING Check this Function Upon Testing
							}
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddNetAddr() Failed in NetAddrArray!");
			}
		}

		//NOTE: 0th Element Can't be removed it is reserved
		//NOTE: Only 1 to TotalNumberOfNetAddr(== (n * MaxUnderflowedFreeSpotsInArray) + 1) can be removed
		void RemoveNetAddr(uint64_t ArrayNumber, bool &IsArrayReordered, bool& IsSuccessful)
		{
			IsArrayReordered = false;
			IsSuccessful = false;
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveNetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (TotalNumberOfReorderedArray > 0)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Remove Element but the previous Reordering of array is not resolved In RemoveNetAddr In: NetAddrArray!\n");
				}
				else
				{
					if (ArrayNumber == 0)
					{
						Essenbp::WriteLogToFile("\n Error Trying to Remove the Reserved Element Number 0 In RemoveNetAddr In: NetAddrArray!\n");
					}
					else
					{
						if (ArrayNumber > (TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray))
						{
							Essenbp::WriteLogToFile("\n Error Trying to Remove non-existant(ArrayNumber > TotalNumberOfNetAddr) element In RemoveNetAddr In: NetAddrArray!\n");
						}
						else
						{
							if (ArrayOfNetAddr[ArrayNumber] == nullptr)
							{
								Essenbp::WriteLogToFile("\n Error The Element At ArrayNumberr " + std::to_string(ArrayNumber) + " Is Empty In RemoveNetAddr In: NetAddrArray!\n");
							}
							else
							{
								if (ArrayNumber > ((TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray) - MaxReservedFreeSpotsInArray))
								{
									ArrayOfNetAddr[ArrayNumber]->FreeOrDeleteCustomStructOrClassptr(CustomStructOrClass_Destructorfunctionptr);
									delete ArrayOfNetAddr[ArrayNumber];
									ArrayOfNetAddr[ArrayNumber] = nullptr;
									//Reorders the the Reserved Array
									uint64_t i = 0;
									for (i = i; i < RemainingNumberOfReservedNetAddarSpotsInArray; ++i)
									{
										if (ArrayNumber > ReservedNetAddarSpotsInArray[i])
										{
											for (uint64_t j = RemainingNumberOfReservedNetAddarSpotsInArray; j > i; --j)
											{
												ReservedNetAddarSpotsInArray[j] = ReservedNetAddarSpotsInArray[j - 1];
											}
											break;
										}
									}
									ReservedNetAddarSpotsInArray[i] = ArrayNumber;
									RemainingNumberOfReservedNetAddarSpotsInArray = RemainingNumberOfReservedNetAddarSpotsInArray + 1;

									IsSuccessful = true;
									TotalNumberOfNetAddr = TotalNumberOfNetAddr - 1;
									//NOTE: This Can not Excede since number greater than the largest reserved number does not exist...
								}
								else
								{
									//NOTE: Since the ArrayNumber Starts always from [1]([0] Is Specially Reserved), The Last ArrayNumber of the Currently Used Slots can be attained by this (TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray - MaxReservedFreeSpotsInArray)
									//NOTE: If the ArrayNumber is Equal to the Last Usable slot, then the slot can be added to reserved slot number [0] and shifting [i] = [i-1] till i > 0  wihout chaning the size of the reserved array
									if (ArrayNumber == ((TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray) - MaxReservedFreeSpotsInArray))
									{
										ArrayOfNetAddr[ArrayNumber]->FreeOrDeleteCustomStructOrClassptr(CustomStructOrClass_Destructorfunctionptr);
										delete ArrayOfNetAddr[ArrayNumber];
										ArrayOfNetAddr[ArrayNumber] = nullptr;
										for (uint64_t i = RemainingNumberOfReservedNetAddarSpotsInArray - 1; i > 0; --i)
										{
											ReservedNetAddarSpotsInArray[i] = ReservedNetAddarSpotsInArray[i - 1];
										}
										ReservedNetAddarSpotsInArray[0] = ArrayNumber;//ArrayNumber == (ReservedNetAddarSpotsInArray[0] - 1)(Assuming RemainingNumberOfReservedNetAddarSpotsInArray == MaxReservedFreeSpotsInArray)

										IsSuccessful = true;
										TotalNumberOfNetAddr = TotalNumberOfNetAddr - 1;
										//NOTE: No need to check for Excede Since nothing is Increased or Decreased, only Shifted
									}
									else
									{
										//NOTE: (Rearranges the Latest Added NetAddr to the Smallest Unused ArrayNumber)
										if ((RemainingNumberOfUnderflowedNetAddarSpotsInArray + 1) >= MaxUnderflowedFreeSpotsInArray)
										{
											NetAddr** TEMPArrayOfNetAddr = nullptr;
											Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPArrayOfNetAddr, (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray), sizeof(NetAddr*), IsSuccessful);
											if (!IsSuccessful)
											{												
												Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray) * sizeof(NetAddr*)) + " Byes Of Memory for TEMPArrayOfNetAddr in AddNetAddr In: NetAddrArray!\n");
											}
											else
											{
												TotalNumberOfNetAddr = TotalNumberOfNetAddr - 1;
												IsArrayReordered = true;
												ArrayOfNetAddr[ArrayNumber]->FreeOrDeleteCustomStructOrClassptr(CustomStructOrClass_Destructorfunctionptr);
												delete ArrayOfNetAddr[ArrayNumber];
												ArrayOfNetAddr[ArrayNumber] = nullptr;
												//Reorders the the Underflowed Array
												uint64_t i = 0;
												for (i = i; i < RemainingNumberOfUnderflowedNetAddarSpotsInArray; ++i)
												{
													if (ArrayNumber > UnderflowedNetAddarSpotsInArray[i])
													{
														for (uint64_t j = RemainingNumberOfUnderflowedNetAddarSpotsInArray; j > i; --j)
														{
															UnderflowedNetAddarSpotsInArray[j] = UnderflowedNetAddarSpotsInArray[j - 1];
														}
														break;
													}
												}
												UnderflowedNetAddarSpotsInArray[i] = ArrayNumber;
												RemainingNumberOfUnderflowedNetAddarSpotsInArray = RemainingNumberOfUnderflowedNetAddarSpotsInArray + 1;

												//This Copies Previous
												for (i = 0; i < ((TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray) - MaxReservedFreeSpotsInArray) + 1; ++i)//NOTE: Array Starts from 0 to n Normally but, 0 is reserved so , 1 to n + 1 length
												{
													TEMPArrayOfNetAddr[i] = ArrayOfNetAddr[i];
												}

												//Sets nullptr for Allocated Space
												for (i = TotalNumberOfNetAddr + 1; i < (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray); ++i)
												{
													TEMPArrayOfNetAddr[i] = nullptr;
												}

												i = 0;
												uint64_t j = TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray;
												while (i < RemainingNumberOfReservedNetAddarSpotsInArray)
												{
													if (ReservedNetAddarSpotsInArray[i] == j)
													{
														j = j - 1;
													}
													else
													{
														i = i + 1;
														if (RemainingNumberOfUnderflowedNetAddarSpotsInArray > 0)
														{
															ReorderedArrayNumbers[(TotalNumberOfReorderedArray * 2)] = j;
															ReorderedArrayNumbers[((TotalNumberOfReorderedArray * 2) + 1)] = UnderflowedNetAddarSpotsInArray[RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1];
															TotalNumberOfReorderedArray = TotalNumberOfReorderedArray + 1;

															TEMPArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1]] = ArrayOfNetAddr[j];
															UnderflowedNetAddarSpotsInArray[RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1] = 0;
															RemainingNumberOfUnderflowedNetAddarSpotsInArray = RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1;
															ArrayOfNetAddr[j] = nullptr;//This is only Important for this Part of code(The else Part)
														}
														else
														{
															i = TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray;
															j = (i + 1) - MaxReservedFreeSpotsInArray;
															uint64_t k = j;
															for (i = i; i > ((TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray) - MaxReservedFreeSpotsInArray); --i)
															{
																if (ArrayOfNetAddr[i] != nullptr)
																{
																	for (j = j; j < (TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray); ++j)
																	{
																		if (ArrayOfNetAddr[j] == nullptr)
																		{
																			ReorderedArrayNumbers[(TotalNumberOfReorderedArray * 2)] = j;
																			ReorderedArrayNumbers[((TotalNumberOfReorderedArray * 2) + 1)] = k;
																			TotalNumberOfReorderedArray = TotalNumberOfReorderedArray + 1;

																			TEMPArrayOfNetAddr[k] = ArrayOfNetAddr[j];
																			k = k + 1;
																			ArrayOfNetAddr[j] = nullptr;
																			break;
																		}
																	}
																}
															}
															break;
														}
														j = j - 1;
													}
												}

												if (RemainingNumberOfUnderflowedNetAddarSpotsInArray > 0)
												{
													i = (TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray) - MaxReservedFreeSpotsInArray;
													while (j > i)
													{
														ReorderedArrayNumbers[(TotalNumberOfReorderedArray * 2)] = j;
														ReorderedArrayNumbers[((TotalNumberOfReorderedArray * 2) + 1)] = UnderflowedNetAddarSpotsInArray[RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1];
														TotalNumberOfReorderedArray = TotalNumberOfReorderedArray + 1;

														TEMPArrayOfNetAddr[UnderflowedNetAddarSpotsInArray[RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1]] = ArrayOfNetAddr[j];
														UnderflowedNetAddarSpotsInArray[RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1] = 0;
														RemainingNumberOfUnderflowedNetAddarSpotsInArray = RemainingNumberOfUnderflowedNetAddarSpotsInArray - 1;
														j = j - 1;
													}
												}

												for (uint64_t i = 0; i < MaxReservedFreeSpotsInArray; ++i)
												{
													ReservedNetAddarSpotsInArray[i] = (TotalNumberOfNetAddr + MaxReservedFreeSpotsInArray) - i;
												}

												RemainingNumberOfReservedNetAddarSpotsInArray = MaxReservedFreeSpotsInArray;
												free(ArrayOfNetAddr);
												ArrayOfNetAddr = TEMPArrayOfNetAddr;
											}
											IsSuccessful = true;
										}
										else
										{
											ArrayOfNetAddr[ArrayNumber]->FreeOrDeleteCustomStructOrClassptr(CustomStructOrClass_Destructorfunctionptr);
											delete ArrayOfNetAddr[ArrayNumber];
											//Reorders the the Underflowed Array
											uint64_t i = 0;
											for (i = i; i < RemainingNumberOfUnderflowedNetAddarSpotsInArray; ++i)
											{
												if (ArrayNumber > UnderflowedNetAddarSpotsInArray[i])
												{
													for (uint64_t j = RemainingNumberOfUnderflowedNetAddarSpotsInArray; j > i; --j)
													{
														UnderflowedNetAddarSpotsInArray[j] = UnderflowedNetAddarSpotsInArray[j - 1];
													}
													break;
												}
											}
											UnderflowedNetAddarSpotsInArray[i] = ArrayNumber;
											RemainingNumberOfUnderflowedNetAddarSpotsInArray = RemainingNumberOfUnderflowedNetAddarSpotsInArray + 1;

											IsSuccessful = true;
											TotalNumberOfNetAddr = TotalNumberOfNetAddr - 1;
											//NOTE: no Need to Check for Excede since it is already checked above
										}
									}
								}
							}
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error RemoveNetAddr() Failed in NetAddrArray!");
			}
		}

		void GetNetAddr(uint64_t ArrayNumber, NetAddr** ReturnNetAddr, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling NetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (TotalNumberOfReorderedArray > 0)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Get Element but the previous Reordering of array is not resolved In GetNetAddr In: NetAddrArray!\n");
				}
				else
				{
					if (ArrayNumber == 0)
					{
						Essenbp::WriteLogToFile("\n Error Trying to Get the Reserved Element Number 0 In GetNetAddr In: NetAddrArray!\n");
						Essenbp::WriteLogToFile("NOTE: Use GetReservedZerothNetAddr() to Get Zeroth([0])th Element In: NetAddrArray!\n");
					}
					else
					{
						if (ArrayNumber < (TotalNumberOfNetAddr + RemainingNumberOfUnderflowedNetAddarSpotsInArray + RemainingNumberOfReservedNetAddarSpotsInArray))
						{
							if (ArrayOfNetAddr[ArrayNumber] == nullptr)
							{
								Essenbp::WriteLogToFile("\n Error The Element At ArrayNumberr " + std::to_string(ArrayNumber) + " Is Empty In GetNetAddr In: NetAddrArray!\n");
							}
							else
							{
								*ReturnNetAddr = ArrayOfNetAddr[ArrayNumber];
								IsSuccessful = true;
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error ArrayNumber Exceeds the Number Of NetAddr Present! in NetAddr In: NetAddrArray!\n");
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error GetNetAddr() Failed in NetAddrArray!");
			}
		}

		uint64_t GetTotalNumberOfNetAddr()
		{
			return TotalNumberOfNetAddr;
		}

		void GetReorderedArrayNumbers(uint64_t* ArgTotalNumberOfReorderedArray, uint64_t** ArgReorderedArrayNumbers, bool ResetArray, bool &IsSuccessful)
		{

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling NetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (TotalNumberOfReorderedArray == 0)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Get Reordered Array Numbers but the Array has been reset since it was manually marked resolved In GetReorderedArrayNumbers In: NetAddrArray!\n");
				}
				else
				{
					*ArgTotalNumberOfReorderedArray = TotalNumberOfReorderedArray;
					*ArgReorderedArrayNumbers = ReorderedArrayNumbers;
					if (ResetArray)
					{
						for (uint64_t i = 0; i < TotalNumberOfReorderedArray; ++i)
						{
							ReorderedArrayNumbers[(i * 2)] = 0;
							ReorderedArrayNumbers[((i * 2) + 1)] = 0;
						}
						TotalNumberOfReorderedArray = 0;
					}
					IsSuccessful = true;
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error RemoveNetAddr() Failed in NetAddrArray!");
			}
		}

		~NetAddrArray()
		{
			Essenbp::WriteLogToFile("\n Destructing NetAddrArray!");
			if (IsConstructionSuccessful)
			{
				if (ArrayOfNetAddr != nullptr)
				{
					for (uint64_t i = 0; i < TotalNumberOfNetAddr; ++i)
					{
						if (ArrayOfNetAddr[i] != nullptr)
						{
							ArrayOfNetAddr[i]->FreeOrDeleteCustomStructOrClassptr(CustomStructOrClass_Destructorfunctionptr);
							delete ArrayOfNetAddr[i];
							ArrayOfNetAddr[i] = nullptr;
						}
					}
					free(ArrayOfNetAddr);
				}
				if (ReservedNetAddarSpotsInArray != nullptr)
				{
					free(ReservedNetAddarSpotsInArray);
					ReservedNetAddarSpotsInArray = nullptr;
				}

				if (UnderflowedNetAddarSpotsInArray != nullptr)
				{
					free(UnderflowedNetAddarSpotsInArray);
					UnderflowedNetAddarSpotsInArray = nullptr;
				}

				if (ReorderedArrayNumbers != nullptr)
				{
					free(ReorderedArrayNumbers);
					ReorderedArrayNumbers = nullptr;
				}

				TotalNumberOfNetAddr = 0;
				RemainingNumberOfReservedNetAddarSpotsInArray = 0;
				RemainingNumberOfUnderflowedNetAddarSpotsInArray = 0;
				TotalNumberOfReorderedArray = 0;

				IsConstructionSuccessful = false;
			}
		}
	};

	enum class NetworkConstructorType 
	{
		TCPIPv4,
		TCPIPv6,
		UDPIPv4,
		UDPIPv6,
		TCPIPv4And6,
		UDPIPv4And6,
		None //Select None If Not needed
	};

	typedef void (*NetworkWrapperCommandFunction)(Essenbp::UnknownDataAndSizeStruct* DataAndSize, NetAddr* ArgNetAddr);
	#define AREA(l, b) (l * b)
	#define NetworkCommandSubSize(DataHolder, Element) (((uint16_t*)(DataHelper->GetData()))[Element])
	#define NavigateDataByByte(DataHolder, Number) (((char*)(DataHolder->GetData())) + Number)

	//NOTE: IPv4 List And IPv6 List both has its own client Numbers, Adding Client to IPv4 will not Increase the List of IPv6 and vice-versa
	struct NetworkWrapper
	{
	public:
		bool IsConstructionSuccessful = false;

	private:

#ifdef _WIN32
		DWORD InputTimeOut = 0;//TCP SOCKET InputTimeOut			//NOTE: will reuse this for UDP custom Time out
		DWORD OutputTimeOut = 0;//TCP SOCKET OutputTimeOut			//NOTE: will reuse this for UDP custom Time out
#else
		unsigned long InputTimeOut = 0;//TCP SOCKET InputTimeOut	//NOTE: will reuse this for UDP custom Time out
		unsigned long OutputTimeOut = 0;//TCP SOCKET OutputTimeOut	//NOTE: will reuse this for UDP custom Time out 
#endif
		uint16_t MaxDataSizePerPacket = 508;//						//NOTE: Optimal Size for UDP is 490 + 18 = 508 And Optimal Size of TCP is 1422 + 19 = 1440 , Minus 10 Extra for NetworkWrapper Specific Info/Commands(8 Byte ClientNumber, 8 Byte Client Unique ID, 2 Byte SizeOfData, 2 Byte Command)
		int MaximumBackLogConnectionsTCP = 5;//						//NOTE: Maximum Number Of TCP Connections That can be Queued At a time
		uint16_t SentPacketsArchiveSize = 256;//					//NOTE: Previous Sent Packets Is Stored(For Each Client)
		uint16_t ReceivedPacketsArchiveSize = 256;//				//NOTE: Previous Sent Packets Is Stored(For Each Client)

		std::atomic_bool IsAddingData = false;//PENDING no processing of data when adding
		std::atomic_bool IsProcessingData = false;//PENDING
		std::atomic_bool IsChangingClientOrder = false;//PENDING

		const bool IsServer;
		bool IsClientTrueForTCPFalseForUDP;// Do not Change it Manually
		uint64_t ClientUniqueIDIPv4;//ONLY FOR CLIENT
		uint64_t ClientUniqueIDIPv6;//ONLY FOR CLIENT

		SOCKET TCPServerSocketIPv4 = NULL;
		SOCKET TCPServerSocketIPv6 = NULL;
		SOCKET UDPServerSocketIPv4 = NULL;
		SOCKET UDPServerSocketIPv6 = NULL;
		sockaddr_in ServerHintIPv4 = { 0 };//This Server or Connecting Server hint
		sockaddr_in6 ServerHintIPv6 = { 0 };//This Server or Connecting Server hint

		//NOTE: This is Only For Server
		NetAddrArray* ArrayOfClients = nullptr;//						//NOTE: This is used by Server for List of Clients Connected
		NetAddrArray* ArrayOfAwaitingConnection = nullptr;//			//NOTE: This is used by Server for List of Awaiting Connections

		//NOTE: This Is Only  For Client
		NetAddr* ThisClient = nullptr;

		//PENDING make a ADDfunction ptr function
		//NOTE: This is where Custom Code goes into(Similar to Event Dispatcher)
		//NOTE: Example Can Be found Below Titled as /*NetworkWrapper Command Number and It's Functions List*/
		NetworkWrapperCommandFunction* ServerCommandFunctionArray;//NOTE: The Function is of Format = void (Essenbp::UnknownDataAndSizeStruct* DataAndSize, NetAddr* ArgNetAddr)
		NetworkWrapperCommandFunction* ClientCommandFunctionArray;//NOTE: The Function is of Format = void (Essenbp::UnknownDataAndSizeStruct* DataAndSize, NetAddr* ArgNetAddr)

		//NOTE: This is used to Subdivide Raw(Data To Send)/Received(Received Data) Data Into Multiple Parts(1,2,3,4,5,...,m Bytes) for processing(If the system is in Little-Endian each subdivided part it reversed) otherwise nothing is done
		Essenbp::ArrayOfUnknownDataAndSize NetworkDataConstructionHelperArray;//NOTE: This is not to be directly used...
		//NOTE: 'n' referts to the number of Types of Received Data Present 
		//NOTE: 'n' = 0 is Reserved it Reverses (8 Byte ClientNumber, 8 Byte Client Unique ID, 2 Byte SizeOfData, 2 Byte Sent/Received Packet Number, 2 Byte Command)
		//NOTE: 'n' = 0 Is for Header Information specific for this NetworkWrapper(NW_P)
		//NOTE: for 'n' from 1 to n(number) will be dedicated for other inputted commands
		//NOTE:	Example: NetworkDataConstructionHelperArray.GetData('n', &ReturnVal, IsSuccessful),
		//NOTE: The First 22(8+8+2+2+2) Bytes of Header will processed using NetworkDataConstructionHelperArray.GetData(0, &ReturnVal, IsSuccessful)
		//NOTE: From the 23rd Byte and onwards the Data will be processed by 'n'. 'n' being any NetworkDataConstructionHelperArray of choice
		//NOTE: Let  k = ((uint16_t*)(ReturnVal->GetData()))[0] Is the TotalNumber Of Subdivided Parts
		//NOTE: Then the Last Element is ((uint16_t*)(ReturnVal->GetData()))[k], Meaning the Max Size of Each element is 2 Bytes(uint16_t)

		//NOTE: For Creating new element or for Adding/Changing SubdivisonByte to Previously Created
		void AddNetworkDataConstructionHelperArrayElement(size_t ElementNumber, uint16_t SubDivisonNumber, uint16_t SubDivisonByte, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddNetworkDataConstructionHelperArrayElement Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				Essenbp::UnknownDataAndSizeStruct* DataHelper = nullptr;
				if (ElementNumber == NetworkDataConstructionHelperArray.GetTotalNumberOfUnknownData())
				{
					if (SubDivisonNumber > 0)
					{
						Essenbp::WriteLogToFile("\n Error SubDivisonNumber Should be 0 when Creating NetworkDataConstructionHelperArrayElement in AddNetworkDataConstructionHelperArrayElement In : NetworkWrapper!\n");
					}
					else
					{
						if (SubDivisonByte == 0)
						{
							Essenbp::WriteLogToFile("\n Error SubDivisonByte should be greater than 0 in AddNetworkDataConstructionHelperArrayElement In : NetworkWrapper!\n");
						}
						else
						{
							if (ElementNumber > 0)
							{
								NetworkDataConstructionHelperArray.GetData((ElementNumber - 1), &DataHelper, IsSuccessful);
								if (IsSuccessful)
								{
									IsSuccessful = (NetworkCommandSubSize(DataHelper,0) > 0);
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n Error The Number of SubDivison Bytes is 0 for Element Number " + std::to_string(ElementNumber - 1) + ", So Add Atleast 1 SubDivisonByte for the Network Data in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
									}
									//else
									//{
									//	//Continued because it is succesful
									//}
								}
								else
								{
									Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.GetData(" + std::to_string(ElementNumber - 1) + ") Failed in AddNetworkDataConstructionHelperArrayElement In : NetworkWrapper!\n");
								}
							}
							else
							{
								IsSuccessful = true;
							}
						}
					}

					if (IsSuccessful)
					{
						NetworkDataConstructionHelperArray.AddElement(IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.AddElement() Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
						}
						else
						{
							NetworkDataConstructionHelperArray.GetData(ElementNumber, &DataHelper, IsSuccessful);
							if (IsSuccessful)
							{
								SubDivisonNumber = 1;//Using it temporarily
								DataHelper->CopyAndStoreData(&SubDivisonNumber, 2, IsSuccessful, false, false);//  2 Bytes for for Total Number
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n DataHelper->CopyAndStoreData(2 Bytes) Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");									
								}
								else
								{
									DataHelper->CopyAndStoreData(&SubDivisonByte, 2, IsSuccessful, false, true);// And 2 Bytes for the First SubDivison Number...
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n DataHelper->CopyAndStoreData(2 Bytes) Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
										NetworkDataConstructionHelperArray.RemoveElement(ElementNumber, IsSuccessful);
										if (!IsSuccessful)
										{
											Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.RemoveElement(" + std::to_string(ElementNumber) + ") Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
										}
										IsSuccessful = false;
									}
								}								
							}
							else
							{
								Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.GetData() Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
								NetworkDataConstructionHelperArray.RemoveElement(ElementNumber, IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.RemoveElement(" + std::to_string(ElementNumber) + ") Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
								}
								IsSuccessful = false;
							}
						}
					}
				}
				else
				{
					if (ElementNumber > NetworkDataConstructionHelperArray.GetTotalNumberOfUnknownData())
					{
						Essenbp::WriteLogToFile("\n Error ElementNumber is greater than NetworkDataConstructionHelperArray.GetTotalNumberOfUnknownData() in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
					}
					else
					{
						NetworkDataConstructionHelperArray.GetData(ElementNumber, &DataHelper, IsSuccessful);
						if (IsSuccessful)
						{
							if ((NetworkCommandSubSize(DataHelper, 0) > SubDivisonNumber))
							{
								NetworkCommandSubSize(DataHelper, (SubDivisonNumber + 1)) = SubDivisonByte;
							}
							else
							{
								if ((NetworkCommandSubSize(DataHelper, 0) == SubDivisonNumber))
								{
									DataHelper->CopyAndStoreData(&SubDivisonByte, 2, IsSuccessful, false, true);
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n DataHelper->CopyAndStoreData(2 Bytes) Failed in AddNetworkDataConstructionHelperArrayElement In: NetworkWrapper!\n");
									}
									else
									{
										NetworkCommandSubSize(DataHelper, 0) = (NetworkCommandSubSize(DataHelper, 0) + 1);
									}
								}
								else
								{
									Essenbp::WriteLogToFile("\n Error The " + std::to_string(SubDivisonNumber) + " is Greater Than The Total Number Of SubDivisons" + std::to_string((((uint16_t*)(DataHelper->GetData()))[0])) + " SubDivisonNumber Should Be Less(When Changing) than or Equal(When Adding One more) to Total Number Of Subdivison for the Network Data in AddNetworkDataConstructionHelperArrayElement In : NetworkWrapper!\n");
									IsSuccessful = false;
								}
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.GetData() Failed in ConvertDataBasedOnNetworkDataConstructionHelperArray In: NetworkWrapper!\n");
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddNetworkDataConstructionHelperArrayElement() Failed In: NetworkWrapper!");
			}
		}

		void RemoveNetworkDataConstructionHelperArrayElement(size_t ElementNumber, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveNetworkDataConstructionHelperArrayElement Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				NetworkDataConstructionHelperArray.RemoveElement(ElementNumber, IsSuccessful);
				if(!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.RemoveElement(" + std::to_string(ElementNumber - 1) + ") Failed in AddNetworkDataConstructionHelperArrayElement In : NetworkWrapper!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error RemoveNetworkDataConstructionHelperArrayElement() Failed In: NetworkWrapper!");
			}
		}

		void ConvertDataToNetWorkByteOrder(void* Data, size_t SizeOfData, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (SizeOfData == 0)
			{
				Essenbp::WriteLogToFile("\n Error SizeOfData is 0 in ConvertDataToNetWorkByteOrder In: NetworkWrapper!\n");
			}
			else
			{
				if (IsLittleEndian)
				{
					Essenbp::ReverseDataOrder(Data, 0, SizeOfData - 1, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Essenbp::ReverseDataOrder() Failed in ConvertDataToNetWorkByteOrder In: NetworkWrapper!");
					}
				}
				else
				{
					IsSuccessful = true;
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ConvertDataToNetWorkByteOrder() Failed In: NetworkWrapper!");
			}
		}

		//NOTE: If the Data is Sent/ReceivedNetWorkData And Of Number > 0, then the Data(Assuming 0 to 21 Byte of Data is Header Info) passed should infact be, Data = ((char*)Source) + 22
		void ConvertDataBasedOnNetworkDataConstructionHelperArray(void* Data, size_t Number, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ConvertDataBasedOnNetworkDataConstructionHelperArray Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (Number < NetworkDataConstructionHelperArray.GetTotalNumberOfUnknownData())
				{
					if (IsLittleEndian)//Only For Little Endian machines
					{
						Essenbp::UnknownDataAndSizeStruct* DataHelper = nullptr;
						NetworkDataConstructionHelperArray.GetData(Number, &DataHelper, IsSuccessful);
						if (IsSuccessful)
						{
							uint64_t CurrentByte = 0;
							uint16_t TotalNumberOfSubdividedParts = NetworkCommandSubSize(DataHelper, 0);
							uint16_t i = 1;
							for (i = 1; i < TotalNumberOfSubdividedParts; ++i)
							{
								ConvertDataToNetWorkByteOrder(((char*)Data + CurrentByte), NetworkCommandSubSize(DataHelper, i), IsSuccessful);//Impossible to Fail So no need for fail check here
								CurrentByte = CurrentByte + NetworkCommandSubSize(DataHelper, i);
							}
							//NOTE: Since [0] is Total Number Of Subdivied Parts, then [1] to [TotalNumberOfSubdividedParts] would be the Last Element
							//NOTE: Doing this to avoid Buffer Overflow(Very unlikely but still possible)
							ConvertDataToNetWorkByteOrder(((char*)Data + CurrentByte), NetworkCommandSubSize(DataHelper, i), IsSuccessful);//Impossible to Fail So no need for fail check here
							CurrentByte = CurrentByte + NetworkCommandSubSize(DataHelper, i);
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error NetworkDataConstructionHelperArray.GetData() Failed in ConvertDataBasedOnNetworkDataConstructionHelperArray In: NetworkWrapper!\n");
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error Number Should be Less than TotalNumber Of NetworkDataConstructionHelperArray in ConvertDataBasedOnNetworkDataConstructionHelperArray In: NetworkWrapper!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ConvertDataBasedOnNetworkDataConstructionHelperArray() Failed In: NetworkWrapper!");
			}
		}

		//NOTE: Only for Received Packets(Server->Client, Client->Server)
		void SwitchBasedOnCommandFromPackage(Essenbp::UnknownDataAndSizeStruct* DataAndSize, NetAddr* ArgNetAddr, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SwitchBasedOnCommandFromPackage Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (DataAndSize == nullptr)
				{
					Essenbp::WriteLogToFile("\n Error DataAndSize is nullptr in SwitchBasedOnCommandFromPackage In: NetworkWrapper!\n");
				}
				else
				{
					if (ArgNetAddr == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ArgNetAddr is nullptr in SwitchBasedOnCommandFromPackage In: NetworkWrapper!\n");
					}
					else
					{
						if (DataAndSize->GetDataSize() < 22)
						{
							Essenbp::WriteLogToFile("\n Error DataAndSize->GetDataSize() is Less than 22 Bytes(Header Size) in SwitchBasedOnCommandFromPackage In: NetworkWrapper!\n");
						}
						else
						{
							ConvertDataBasedOnNetworkDataConstructionHelperArray((DataAndSize->GetData()), 0, IsSuccessful);//0 is for Header Information. Header Info is First converted based on endian of the machine
							if (IsSuccessful)
							{
								if (DataAndSize->GetDataSize() > 22)
								{
									ConvertDataBasedOnNetworkDataConstructionHelperArray((DataAndSize->GetData()), *(uint16_t*)NavigateDataByByte(DataAndSize, 20), IsSuccessful);
								}
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("Error ConvertDataBasedOnNetworkDataConstructionHelperArray() Failed in SwitchBasedOnCommandFromPackage In: NetworkWrapper!");
								}
								else
								{
									if (IsServer)
									{
										ServerCommandFunctionArray[*(uint16_t*)NavigateDataByByte(DataAndSize, 20)](DataAndSize, ArgNetAddr);
									}
									else
									{
										ClientCommandFunctionArray[*(uint16_t*)NavigateDataByByte(DataAndSize, 20)](DataAndSize, ArgNetAddr);
									}
								}								
							}
							else
							{
								Essenbp::WriteLogToFile("Error ConvertDataBasedOnNetworkDataConstructionHelperArray() Failed in SwitchBasedOnCommandFromPackage In: NetworkWrapper!");
							}
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SwitchBasedOnCommandFromPackage() Failed In: NetworkWrapper!");
			}
		}

		//PENDING add Atomic Bool for this
		//NOTE: 2 Byte Command And Remaining Data //NOTE:8 Byte Client Number And Client Unique Number should not be Added here since it is constant...
		//NOTE: DataSize = 2 Byte + Remaining Data size
		void ClientAddSentPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ClientAddSentPackage Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Send Data to Server from A Client in ClientAddSentPackage In: NetworkWrapper!\n");
					Essenbp::WriteLogToFile("NOTE: Construct a Client To Send Data to Host Server\n");
				}
				else
				{
					ThisClient->AddSentPackage(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error NetAddr::AddSentPackage Failed in ClientAddSentPackage In: NetworkWrapper!");
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ClientAddSentPackage() Failed In: NetworkWrapper!");
			}
		}

		//PENDING add Atomic Bool for this
		//NOTE: 2 Byte Command And Remaining Data //NOTE:8 Byte Client Number And Client Unique Number should not be Added here since it is constant...
		//NOTE: DataSize = 2 Byte + Remaining Data size
		void ClientAddReceivedPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ClientAddReceivedPackage Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Send Data to Server from A Client in ClientAddReceivedPackage In: NetworkWrapper!\n");
					Essenbp::WriteLogToFile("NOTE: Construct a Client To Send Data to Host Server\n");
				}
				else
				{
					ThisClient->AddReceivedPackage(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error NetAddr::AddReceivedPackage Failed in ClientAddReceivedPackage In: NetworkWrapper!");
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ClientAddReceivedPackage() Failed In: NetworkWrapper!");
			}
		}

		//NOTE: This checks if the sender is in List, and has given correct ClientUniqueID[8 to 15] ("Password" Randomly Generated upon successful connection to The server)
		//NOTE: Received_SizeOfData is the Size of the Data(Designated by the Server/Client) Carried by the Received packet
		//NOTE: If The ClientReturnPtr is not needed pass 'nullptr'
		void CheckReceivedDataInfoServer(char* ReceivedData, uint16_t& Received_SizeOfData, sockaddr_in* ReceivedAddress, NetAddr** ClientReturnPtr, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling CheckReceivedDataInfoServer Without Constructing the struct In: NetworkWrapper!\n");
				Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed In: NetworkWrapper!");
			}
			else
			{		
				bool ValidClientReturnPtr = true;
				if (ClientReturnPtr == nullptr)
				{
					ClientReturnPtr = (NetAddr**)malloc(sizeof(NetAddr*));
					if (ClientReturnPtr != nullptr)
					{
						ValidClientReturnPtr = false;
					}
				}
				uint64_t Received_ClientNumber = ntohll(((uint64_t*)ReceivedData)[0]);			// [0] to [7] Char
				uint64_t Received_ClientUniqueID = ntohll(((uint64_t*)ReceivedData)[1]);		// [8] to [15] Char
				Received_SizeOfData = ntohll(((uint16_t*)ReceivedData)[8]);						//[16] to [17] Char
				//uint16_t Received_NetworkWrapperCommand = ntohll(((uint16_t*)ReceivedData)[9]);	//[18] to [19] Char //NOT NEEDED HERE!

				ArrayOfClients->GetNetAddr(Received_ClientNumber, ClientReturnPtr, IsSuccessful);
				if (IsSuccessful)
				{
					if ((*ClientReturnPtr)->GetClientUniqueID() == Received_ClientUniqueID)
					{
						if ((*ClientReturnPtr)->TrueForIPv6FalseForIPv4)
						{
							IsSuccessful = false;
						}
						else
						{
							CompareIPAddr((sockaddr_in*)(*ClientReturnPtr)->IPAddr, ReceivedAddress, IsSuccessful);
						}						
					}
					else
					{
						IsSuccessful = false;
					}
				}

				if (!ValidClientReturnPtr)
				{
					free(ClientReturnPtr);
				}
			}

			//if (!IsSuccessful)//NO Need here because This is Received Data From The Internet...  //CHECK PENDING
			//{
			//	Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed In: NetworkWrapper!");
			//}
		}
		void CheckReceivedDataInfoServer(char* ReceivedData, uint16_t& Received_SizeOfData, sockaddr_in6* ReceivedAddress, NetAddr** ClientReturnPtr, bool& IsSuccessful)
		{			
			IsSuccessful = false;			

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling CheckReceivedDataInfoServer Without Constructing the struct In: NetworkWrapper!\n");
				Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed In: NetworkWrapper!");
			}
			else
			{
				bool ValidClientReturnPtr = true;
				if (ClientReturnPtr == nullptr)
				{
					ClientReturnPtr = (NetAddr**)malloc(sizeof(NetAddr*));
					if (ClientReturnPtr != nullptr)
					{
						ValidClientReturnPtr = false;
					}
				}

				uint64_t Received_ClientNumber = ntohll(((uint64_t*)ReceivedData)[0]);			// [0] to [7] Char
				uint64_t Received_ClientUniqueID = ntohll(((uint64_t*)ReceivedData)[1]);		// [8] to [15] Char
				Received_SizeOfData = ntohll(((uint16_t*)ReceivedData)[8]);						//[16] to [17] Char
				//uint16_t Received_NetworkWrapperCommand = ntohll(((uint16_t*)ReceivedData)[9]);	//[18] to [19] Char //NOT NEEDED HERE!

				ArrayOfClients->GetNetAddr(Received_ClientNumber, ClientReturnPtr, IsSuccessful);
				if (IsSuccessful)
				{
					if ((*ClientReturnPtr)->GetClientUniqueID() == Received_ClientUniqueID)
					{
						if ((*ClientReturnPtr)->TrueForIPv6FalseForIPv4)
						{
							CompareIPAddr((sockaddr_in6*)(*ClientReturnPtr)->IPAddr, ReceivedAddress, IsSuccessful);
						}
						else
						{
							IsSuccessful = false;
						}
					}
					else
					{
						IsSuccessful = false;
					}
				}

				if (!ValidClientReturnPtr)
				{
					free(ClientReturnPtr);
				}
			}

			//if (!IsSuccessful)//NO Need here because This is Received Data From The Internet...  //CHECK PENDING
			//{
			//	Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed In: NetworkWrapper!");
			//}
		}

		void SendDataUDP(const sockaddr_in* DestinationAddress, Essenbp::UnknownDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendDataUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (UDPServerSocketIPv4 == NULL)
				{
					Essenbp::WriteLogToFile("\n Error IPv4 Socket Is not Created in SendDataUDP In: NetworkWrapper!\n");
				}
				else
				{
					sendto(UDPServerSocketIPv4, (char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), 0, (sockaddr*)DestinationAddress, sizeof(*DestinationAddress));
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendDataUDP Failed In: NetworkWrapper!");
			}
		}		

		void SendDataUDP(const sockaddr_in6* DestinationAddress, Essenbp::UnknownDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendDataUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (UDPServerSocketIPv6 == NULL)
				{
					Essenbp::WriteLogToFile("\n Error IPv6 Socket Is not Created in SendDataUDP In: NetworkWrapper!\n");
				}
				else
				{
					sendto(UDPServerSocketIPv6, (char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), 0, (sockaddr*)DestinationAddress, sizeof(*DestinationAddress));
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendDataUDP Failed In: NetworkWrapper!");
			}
		}

		//NOTE:From Server = TCP ONly
		//NOTE:From Client = UDP(Connected) And TCP
		void SendDataTCPUDP(SOCKET DestinationSOCKET, Essenbp::UnknownDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendDataTCPUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				send(DestinationSOCKET, (char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), 0);
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendDataTCPUDP Failed In: NetworkWrapper!");
			}
		}

		void ReceiveData(SOCKET ReciveSocket, char* ReceivedData, uint16_t DataBufferSize, uint16_t& SizeOfReturnedData, bool& IsSuccessful)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ReceiveData Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				memset(ReceivedData, 0, DataBufferSize); // Clear the receive Buffer(ReceivedDatafer)			
				SizeOfReturnedData = recv(ReciveSocket, ReceivedData, sizeof(ReceivedData), 0);

				if ((SizeOfReturnedData == 0) || (WSAGetLastError() > 0))// When the size is 0
				{
					Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveData In: NetworkWrapper!");
					Essenbp::WriteLogToFile("\n Error ReceiveData() Failed with Error " + std::to_string(WSAGetLastError()) + " in ReceiveData In: NetworkWrapper!");
				}
			}
		}

		void ReceiveData(SOCKET ReciveSocket, char* ReceivedData, uint16_t DataBufferSize, uint16_t& SizeOfReturnedData, bool& IsSuccessful, sockaddr_in* IPv4Address)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ReceiveData Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				int SockAddrSize = sizeof(sockaddr*);
				memset(ReceivedData, 0, DataBufferSize); // Clear the receive Buffer(ReceivedDatafer)			
				SizeOfReturnedData = recvfrom(ReciveSocket, ReceivedData, sizeof(ReceivedData), 0, (sockaddr*)IPv4Address, &SockAddrSize);

				if ((SizeOfReturnedData == 0) || (WSAGetLastError() > 0))// When the size is 0
				{
					Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveData In: NetworkWrapper!");
					Essenbp::WriteLogToFile("\n Error ReceiveData() Failed with Error " + std::to_string(WSAGetLastError()) + " in ReceiveData In: NetworkWrapper!");
				}
			}
		}

		void ReceiveData(SOCKET ReciveSocket, char* ReceivedData, uint16_t DataBufferSize, uint16_t& SizeOfReturnedData, bool& IsSuccessful, sockaddr_in6* IPv6Address)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ReceiveData Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				int SockAddrSize = sizeof(sockaddr*);
				memset(ReceivedData, 0, DataBufferSize); // Clear the receive Buffer(ReceivedDatafer)			
				SizeOfReturnedData = recvfrom(ReciveSocket, ReceivedData, sizeof(ReceivedData), 0, (sockaddr*)IPv6Address, &SockAddrSize);

				if ((SizeOfReturnedData == 0) || (WSAGetLastError() > 0))// When the size is 0
				{
					Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveData In: NetworkWrapper!");
					Essenbp::WriteLogToFile("\n Error ReceiveData() Failed with Error " + std::to_string(WSAGetLastError()) + " in ReceiveData In: NetworkWrapper!");
				}
			}
		}

	public:
		//For Server to Client
		//NOTE: For TrueForConnectedClientFalseForAwaitingConnection, True when the Client is Fully Connected, False when the Client is Still in the process of Connecting
		void SendData(bool TrueForConnectedClientFalseForAwaitingConnection, uint64_t ClientNumber, Essenbp::UnknownDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendData Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (!IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Send Data to Client from A Client in SendData In: NetworkWrapper!\n");
					Essenbp::WriteLogToFile("NOTE: Construct a Server To Send Data to Connected Clients\n");
				}
				else
				{
					NetAddr* Clientptr = nullptr;
					if (TrueForConnectedClientFalseForAwaitingConnection)
					{
						ArrayOfClients->GetNetAddr(ClientNumber, &Clientptr, IsSuccessful);
					}
					else
					{
						ArrayOfAwaitingConnection->GetNetAddr(ClientNumber, &Clientptr, IsSuccessful);
					}
					
					if (IsSuccessful)
					{
						if (Clientptr->GetSocket() != NULL)
						{
							ClientAddSentPackage((char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("Error ClientAddSentPackage() Failed in SendData In: NetworkWrapper!");
							}
							else
							{
								SendDataTCPUDP(Clientptr->GetSocket(), DataAndSize, IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
								}
							}
						}
						else
						{
							ClientAddSentPackage((char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("Error ClientAddSentPackage() Failed in SendData In: NetworkWrapper!");
							}
							else
							{
								if (Clientptr->TrueForIPv6FalseForIPv4)
								{
									SendDataUDP(&(((NetAddrIPv6*)(Clientptr->IPAddr))->NetAddress), DataAndSize, IsSuccessful);
								}
								else
								{
									SendDataUDP(&(((NetAddrIPv4*)(Clientptr->IPAddr))->NetAddress), DataAndSize, IsSuccessful);
								}
							}

							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error SendDataUDP() Failed in SendData In: NetworkWrapper!");
							}
						}
					}
					else
					{
						Essenbp::WriteLogToFile("\n Error NetAddrArray::GetNetAddr() Failed in SendData In: NetworkWrapper!");
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendData Failed In: NetworkWrapper!");
			}
		}

		//For Client To Server
		void SendData(Essenbp::UnknownDataAndSizeStruct& DataAndSize, bool TrueForIPv6FalseForIPv4, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendData Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Send Data to Server from A Client in SendData In: NetworkWrapper!\n");
					Essenbp::WriteLogToFile("NOTE: Construct a Client To Send Data to Host Server\n");
				}
				else
				{
					if (TrueForIPv6FalseForIPv4)
					{
						ClientAddSentPackage((char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("Error ClientAddSentPackage() Failed in SendData In: NetworkWrapper!");
						}
						else
						{
							if (IsClientTrueForTCPFalseForUDP)
							{
								SendDataTCPUDP(TCPServerSocketIPv6, DataAndSize, IsSuccessful);
							}
							else
							{
								SendDataTCPUDP(UDPServerSocketIPv6, DataAndSize, IsSuccessful);
							}
						}
						
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
						}
					}
					else
					{
						ClientAddSentPackage((char*)(DataAndSize.GetData()), DataAndSize.GetDataSize(), IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("Error ClientAddSentPackage() Failed in SendData In: NetworkWrapper!");
						}
						else
						{
							if (IsClientTrueForTCPFalseForUDP)
							{
								SendDataTCPUDP(TCPServerSocketIPv4, DataAndSize, IsSuccessful);
							}
							else
							{
								SendDataTCPUDP(UDPServerSocketIPv4, DataAndSize, IsSuccessful);
							}
						}

						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
						}
					}
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
					}					
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendData Failed In: NetworkWrapper!");
			}
		}	
	
	private:
										/*NetworkWrapper Command Number and It's Functions List*///PENDING ADD PACKET NUMBER
		/****************************************************************************************************************************///PENDING
		//		NetworkWrapper Specific Info/Commands = (8 Byte ClientNumber, 8 Byte Client Unique ID, 2 Byte SizeOfData, 2 Byte Sent/Received Packet Number, 2 Byte Command)
		//					 ClientUniqueID[8 to 15] ("Password" Randomly Generated upon successful connection to The server)
		//						   Network Wrapper Command Byte Meaning (ReceivedData[20] & [21] Is the Command Byte)
		//											   Functions Defination Declared At the Last				
		// 
		//
		//														/*Server*/
		/*
		* 0.) Connect to Server Initial/Confirmation
		* 1.) Disconnection Initial/Confirmation
		* //2.) Normal Passage Of Data(+ 2 Bytes Command)//PENDING
		*/
		//
		//
		//														/*CLIENT*/
		/*
		* 0.) Connect to Server Initial/Confirmation
		* 1.) Disconnection Intial/Confirmation
		* //1.) Normal Passage Of Data(+ 2 Bytes Command)//PENDING
		*/
		/****************************************************************************************************************************/
							
										/*NetworkWrapper Command Number Specific Functions Defination List*/
		/****************************************************************************************************************************///PENDING
		//													/*Server*/
		//Command = 0, Verify Connection of Client to Server / Disconnect Client from Server
		/*void ServerSideConnectionDisconnectionConfirmationTCP(SOCKET ClientSocket, bool& IsSuccessful, bool IsInitialJoin = false, uint8_t MaxVerificationTries = 2)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling  ServerSideConnectionDisconnectionConfirmationTCP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying Use Server Side Version from Client in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
				}
				else
				{
					Essenbp::UnknownDataAndSizeStruct DataAndSize;
					char* SendDataChar = (char*)malloc(24);
					if (SendDataChar == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error Allocating 24 Byes Of Memory for SendDataChar in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
					}
					else
					{
						memset(SendDataChar, 0, 24);
						//\*((uint16_t*)( SendDataChar + 0 ) ) = 0;				//Not Needed Here
						//\*((uint16_t*)( SendDataChar + 8 ) ) = 0;				//Not Needed Here
						*((uint16_t*)( SendDataChar + 16) ) = 4;				//Size
						*((uint16_t*)( SendDataChar + 18) ) = 2;				//Command
						*((uint32_t*)( SendDataChar + 20) ) = std::rand();		//Temporary Password To Join/Exit Server
						DataAndSize.CopyAndStoreData(SendDataChar, 24, IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error DataAndSize.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
						}
						else
						{
							DataAndSize.CopyAndStoreData((SendDataChar + 16), 2, IsSuccessful, false, true);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error DataAndSize.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
							}
							else
							{
								DataAndSize.CopyAndStoreData((SendDataChar + 18), 2, IsSuccessful, false, true);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error DataAndSize.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
								}
								else
								{
									DataAndSize.CopyAndStoreData((SendDataChar + 20), 4, IsSuccessful, false, true);
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n Error DataAndSize.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
									}
								}
							}
						}
						free(SendDataChar);

						if (IsSuccessful)
						{
							SendDataTCPUDP(&ClientSocket, DataAndSize, IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error SendDataTCPUDP Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
							}
							else
							{
								IsSuccessful = false;
								char* ReceivedData = (char*)malloc(MaxDataSizePerPacket);
								if (ReceivedData == nullptr)
								{
									Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(MaxDataSizePerPacket) + " Byes Of Memory for ReceivedData in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");									
								}
								else
								{		
									uint16_t Received_SizeOfData = 0;
									ReceiveData(ClientSocket, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
									if (!IsSuccessful)// When the size is 0
									{
										Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!");
									}
									else
									{
										IsSuccessful = false;
										for (uint8_t i = 0; i < MaxVerificationTries; ++i)
										{
											if(!IsInitialJoin)
											{
												IsSuccessful = true;
											}
											else
											{
												CheckReceivedDataInfoServer(ReceivedData, false, Received_SizeOfData, IsSuccessful);
											}

											if (!IsSuccessful)
											{
												Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!");
											}
											else
											{
												IsSuccessful = false;
												SendDataChar = DataAndSize.GetData();
												if (IsLittleEndian)
												{
													//Checks Not Needed Below Since Correct Values are Inputted
													//Essenbp::ReverseDataOrder((void*)ReceivedData, 0, 7, IsSuccessful);		//Not Needed Here
													//Essenbp::ReverseDataOrder((void*)ReceivedData, 8, 15, IsSuccessful);	//Not Needed Here
													Essenbp::ReverseDataOrder((void*)ReceivedData, 16, 18, IsSuccessful);	//Size
													Essenbp::ReverseDataOrder((void*)ReceivedData, 19, 20, IsSuccessful);	//Command
													Essenbp::ReverseDataOrder((void*)ReceivedData, 21, 24, IsSuccessful);	//Temporary Password To Join/Exit Server
												}
												if (*((uint16_t*)(ReceivedData + 16)) == 4)//*((uint16_t*)(SendDataChar + 16)))
												{
													if (*((uint32_t*)(ReceivedData + 18)) == 0)//*((uint32_t*)(SendDataChar + 18)))
													{
														if (*((uint32_t*)(ReceivedData + 20)) == *((uint32_t*)(SendDataChar + 20)))
														{
															IsSuccessful = true;
															break;
														}
														else
														{
															Essenbp::WriteLogToFile("\n Error *((uint32_t*)(ReceivedData + 20)) is not Equal to *((uint32_t*)(SendDataChar + 20)) Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
														}
													}
													else
													{
														Essenbp::WriteLogToFile("\n Error *((uint32_t*)(ReceivedData + 18)) is not Equal to 0 Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
													}
												}
												else
												{
													Essenbp::WriteLogToFile("\n Error *((uint16_t*)(ReceivedData + 16)) is not Equal to 4 Failed in ServerSideConnectionDisconnectionConfirmationTCP In: NetworkWrapper!\n");
												}
											}
										}
									}
									free(ReceivedData);
								}								
							}
						}
					}
				}
			}
		}
		void ServerSideConnectionDisconnectionConfirmationUDP(bool SendTempPassword_Or_VerifyReceivedTempPassword, const sockaddr_in* DestinationAddress, Essenbp::UnknownDataAndSizeStruct& SentDataOrReceivedData, uint32_t& TempPassword, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling  ServerSideConnectionDisconnectionConfirmationUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying Use Server Side Version from Client in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
				}
				else
				{
					if (SendTempPassword_Or_VerifyReceivedTempPassword)
					{
						char* SendDataChar = (char*)malloc(24);
						if (SendDataChar == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating 24 Byes Of Memory for SendDataChar in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
						}
						else
						{
							memset(SendDataChar, 0, 24);
							//\*((uint16_t*)( SendDataChar + 0 ) ) = 0;				//Not Needed Here
							//\*((uint16_t*)( SendDataChar + 8 ) ) = 0;				//Not Needed Here
							*((uint16_t*)(SendDataChar + 16)) = 4;					//Size
							*((uint16_t*)(SendDataChar + 18)) = 2;					//Command
							*((uint32_t*)(SendDataChar + 20)) = std::rand();		//Temporary Password To Join/Exit Server
							SentDataOrReceivedData.CopyAndStoreData(SendDataChar, 24, IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
							}
							else
							{
								SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 16), 2, IsSuccessful, false, true);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
								}
								else
								{
									SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 18), 2, IsSuccessful, false, true);
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
									}
									else
									{
										SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 20), 4, IsSuccessful, false, true);
										if (!IsSuccessful)
										{
											Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
										}
									}
								}
							}
							free(SendDataChar);

							if (IsSuccessful)
							{
								SendDataUDP(DestinationAddress, SentDataOrReceivedData, IsSuccessful);
								if (!IsSuccessful)
								{
									TempPassword = *((uint32_t*)(SendDataChar + 20));
									Essenbp::WriteLogToFile("\n Error SendDataUDP Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
								}
							}
						}
					}
					else
					{			
						if (IsLittleEndian)
						{
							//Checks Not Needed Below Since Correct Values are Inputted
							//Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 0, 7, IsSuccessful);		//Not Needed Here
							//Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 8, 15, IsSuccessful);	//Not Needed Here
							Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 16, 18, IsSuccessful);	//Size
							Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 19, 20, IsSuccessful);	//Command
							Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 21, 24, IsSuccessful);	//Temporary Password To Join/Exit Server
						}
						IsSuccessful = false;
						if (*((uint16_t*)(SentDataOrReceivedData.GetData() + 16)) == 4)//*((uint16_t*)(SendDataChar + 16)))
						{
							if (*((uint32_t*)(SentDataOrReceivedData.GetData() + 18)) == 0)//*((uint32_t*)(SendDataChar + 18)))
							{
								if (*((uint32_t*)(SentDataOrReceivedData.GetData() + 20)) == TempPassword)
								{
									IsSuccessful = true;
								}
								else
								{
									Essenbp::WriteLogToFile("\n Error *((uint32_t*)(ReceivedData + 20)) is not Equal to *((uint32_t*)(SendDataChar + 20)) Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
								}
							}
							else
							{
								Essenbp::WriteLogToFile("\n Error *((uint32_t*)(ReceivedData + 18)) is not Equal to 0 Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error *((uint16_t*)(ReceivedData + 16)) is not Equal to 4 Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
						}
					}
				}
			}
		}
		void ServerSideConnectionDisconnectionConfirmationUDP(bool SendTempPassword_Or_VerifyReceivedTempPassword, const sockaddr_in6* DestinationAddress, Essenbp::UnknownDataAndSizeStruct& SentDataOrReceivedData, uint32_t& TempPassword, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling  ServerSideConnectionDisconnectionConfirmationUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying Use Server Side Version from Client in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
				}
				else
				{
					if (SendTempPassword_Or_VerifyReceivedTempPassword)
					{
						char* SendDataChar = (char*)malloc(24);
						if (SendDataChar == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating 24 Byes Of Memory for SendDataChar in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
						}
						else
						{
							memset(SendDataChar, 0, 24);
							//\*((uint16_t*)( SendDataChar + 0 ) ) = 0;				//Not Needed Here
							//\*((uint16_t*)( SendDataChar + 8 ) ) = 0;				//Not Needed Here
							*((uint16_t*)(SendDataChar + 16)) = 4;					//Size
							*((uint16_t*)(SendDataChar + 18)) = 2;					//Command
							*((uint32_t*)(SendDataChar + 20)) = std::rand();		//Temporary Password To Join/Exit Server
							SentDataOrReceivedData.CopyAndStoreData(SendDataChar, 24, IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
							}
							else
							{
								SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 16), 2, IsSuccessful, false, true);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
								}
								else
								{
									SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 18), 2, IsSuccessful, false, true);
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
									}
									else
									{
										SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 20), 4, IsSuccessful, false, true);
										if (!IsSuccessful)
										{
											Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
										}
									}
								}
							}
							free(SendDataChar);

							if (IsSuccessful)
							{
								SendDataUDP(DestinationAddress, SentDataOrReceivedData, IsSuccessful);
								if (!IsSuccessful)
								{
									TempPassword = *((uint32_t*)(SendDataChar + 20));
									Essenbp::WriteLogToFile("\n Error SendDataUDP Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
								}
							}
						}
					}
					else
					{
						if (IsLittleEndian)
						{
							//Checks Not Needed Below Since Correct Values are Inputted
							//Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 0, 7, IsSuccessful);		//Not Needed Here
							//Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 8, 15, IsSuccessful);	//Not Needed Here
							Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 16, 18, IsSuccessful);	//Size
							Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 19, 20, IsSuccessful);	//Command
							Essenbp::ReverseDataOrder((void*)SentDataOrReceivedData.GetData(), 21, 24, IsSuccessful);	//Temporary Password To Join/Exit Server
						}
						IsSuccessful = false;
						if (*((uint16_t*)(SentDataOrReceivedData.GetData() + 16)) == 4)//*((uint16_t*)(SendDataChar + 16)))
						{
							if (*((uint32_t*)(SentDataOrReceivedData.GetData() + 18)) == 0)//*((uint32_t*)(SendDataChar + 18)))
							{
								if (*((uint32_t*)(SentDataOrReceivedData.GetData() + 20)) == TempPassword)
								{
									IsSuccessful = true;
								}
								else
								{
									Essenbp::WriteLogToFile("\n Error *((uint32_t*)(ReceivedData + 20)) is not Equal to *((uint32_t*)(SendDataChar + 20)) Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
								}
							}
							else
							{
								Essenbp::WriteLogToFile("\n Error *((uint32_t*)(ReceivedData + 18)) is not Equal to 0 Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error *((uint16_t*)(ReceivedData + 16)) is not Equal to 4 Failed in ServerSideConnectionDisconnectionConfirmationUDP In: NetworkWrapper!\n");
						}
					}
				}
			}
		}*/	

		//NOTE: Use this as an Example to Create More ServerCommandFunctionArray
		//NOTE: The Data Should be Reversed using NetworkDataConstructionHelperArray[CommandNumber]
		void ServerSideConnectionDisconnectionConfirmation(Essenbp::UnknownDataAndSizeStruct* DataAndSize, NetAddr* ArgNetAddr)
		{
			//PENDING REVAMP THIS
			bool IsSuccessful = false;
			//if ((uint64_t*)(((char*)(DataAndSize->GetData())) + 0) == 0)
			if(((uint64_t*)(NavigateDataByByte(DataAndSize, 0))) == 0)//Check if the ClientNumber is 0 or not, if it is 0 it means a New client wants to connect, If not then the Client is In Pending List(Confirmation Stage)
			{
				//PENDING
				if (ArgNetAddr->TrueForIPv6FalseForIPv4)
				{
					ArrayOfAwaitingConnection->AddNetAddr(ArgNetAddr->GetSocket(), *((sockaddr_in6*)(ArgNetAddr->IPAddr)), IsSuccessful);
				}
				else
				{
					ArrayOfAwaitingConnection->AddNetAddr(ArgNetAddr->GetSocket(), *((sockaddr_in*)(ArgNetAddr->IPAddr)), IsSuccessful);
				}

				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error NetAddrArray::AddNetAddr() Failed in ServerSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
				}
				else
				{
					SendData(false, )
					//PENDING
				}
				//PENDING NOW SEND THIS TO THE CLIENT 
			}
			else//This means Current Client wants to disconnect
			{

			}
			NetworkCommandSubSize(DataHelper, i);
			//PENDING
			//ACTUAL CODE STARTS HERE				
		}
		

	
		//Command = 1, Normal Passage Of Data(Total Data Size = Data Size + 2 Bytes Command)
		//PENDING

															/*CLIENT*/
		//Command = 0, Connect to Server / Disconnect from Server Request/Confirmation
		void ClientSideConnectionDisconnectionConfirmation(bool GetTempPassword_or_SendBackTempPassword, bool TrueForIPv6FalseForIPv4, uint32_t& TempPassword, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ClientSideConnectionDisconnectionConfirmation Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying Use Client Side Version from Server in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
				}
				else
				{
					Essenbp::UnknownDataAndSizeStruct SentDataOrReceivedData;
					char* SendDataChar = (char*)malloc(24);
					if (SendDataChar == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error Allocating 24 Byes Of Memory for SendDataChar in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
					}
					else
					{
						memset(SendDataChar, 0, 24);
						//\*((uint16_t*)( SendDataChar + 0 ) ) = 0;				//Not Needed Here
						//\*((uint16_t*)( SendDataChar + 8 ) ) = 0;				//Not Needed Here
						*((uint16_t*)(SendDataChar + 16)) = 4;					//Size
						*((uint16_t*)(SendDataChar + 18)) = 2;					//Command
						if (GetTempPassword_or_SendBackTempPassword)
						{
							*((uint32_t*)(SendDataChar + 20)) = 0;//Server Decides	//Temporary Password To Join/Exit Server
						}
						else
						{
							*((uint32_t*)(SendDataChar + 20)) = TempPassword;//Received Temporary Password To Join/Exit Server
						}
						SentDataOrReceivedData.CopyAndStoreData(SendDataChar, 16, IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
						}
						else
						{
							SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 16), 2, IsSuccessful, false, true);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
							}
							else
							{
								SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 18), 2, IsSuccessful, false, true);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
								}
								else
								{
									SentDataOrReceivedData.CopyAndStoreData((SendDataChar + 20), 4, IsSuccessful, false, true);
									if (!IsSuccessful)
									{
										Essenbp::WriteLogToFile("\n Error SentDataOrReceivedData.CopyAndStoreData() Failed in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
									}
									else
									{
										SendData(SentDataOrReceivedData, TrueForIPv6FalseForIPv4, IsSuccessful);
										if (!IsSuccessful)
										{
											Essenbp::WriteLogToFile("\n Error SendData Failed in ClientSideConnectionDisconnectionConfirmation In: NetworkWrapper!\n");
										}
									}
								}
							}
						}
						free(SendDataChar);						
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error IntialClientJoinToServerThreadIPv4TCP Failed In: NetworkWrapper!");
			}
		}

		//Command = 1, Normal Passage Of Data(Total Data Size = Data Size + 2 Bytes Command)
		//PENDING
		/****************************************************************************************************************************/		

		//TCP Initial Join Thread, To Be Stored In An Array
		void IntialClientJoinToServerThreadIPv4TCP(SOCKET ClientSocket, sockaddr_in ClientAddress)
		{
			bool IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling IntialClientJoinToServerThreadIPv4TCP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				int ClientLength = sizeof(ClientAddress); // The size of the client information	
				char* ReceivedData = (char*)malloc(MaxDataSizePerPacket);

				uint16_t Received_SizeOfData = 0;

				if (ReceivedData == nullptr)
				{
					Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(MaxDataSizePerPacket) + " Byes Of Memory for ReceivedData in IntialClientJoinToServerThreadIPv4TCP In: NetworkWrapper!\n");
				}
				else
				{
					setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&InputTimeOut, sizeof(InputTimeOut));
					setsockopt(ClientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&OutputTimeOut, sizeof(OutputTimeOut));

					// Initial Check
					memset(ReceivedData, 0, MaxDataSizePerPacket); // Clear the receive Buffer(ReceivedDatafer)		
					ReceiveData(ClientSocket, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);

					if (!IsSuccessful)// When the size is 0
					{
						Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in IntialClientJoinToServerThreadIPv4TCP In: NetworkWrapper!");
						closesocket(ClientSocket);
					}
					else
					{
						ServerSideConnectionDisconnectionConfirmationTCP(ClientSocket, IsSuccessful, true, 2);
						if (!IsSuccessful)
						{
							closesocket(ClientSocket);
						}
						else
						{
							AddClientIPv4(&ClientSocket, &ClientAddress, IsSuccessful);//PENDING make it a Queue
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error IntialClientJoinToServerThreadIPv4TCP Failed In: NetworkWrapper!");
			}
		}

		//TCP Initial Join Thread, To Be Stored In An Array
		void IntialClientJoinToServerThreadIPv6TCP(SOCKET ClientSocket, sockaddr_in6 ClientAddress)
		{
			bool IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling IntialClientJoinToServerThreadIPv6TCP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				int ClientLength = sizeof(ClientAddress); // The size of the client information	
				char* ReceivedData = (char*)malloc(MaxDataSizePerPacket);

				uint16_t Received_SizeOfData = 0;

				if (ReceivedData == nullptr)
				{
					Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(MaxDataSizePerPacket) + " Byes Of Memory for ReceivedData in IntialClientJoinToServerThreadIPv6TCP In: NetworkWrapper!\n");
				}
				else
				{
					setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&InputTimeOut, sizeof(InputTimeOut));
					setsockopt(ClientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&OutputTimeOut, sizeof(OutputTimeOut));

					// Initial Check
					memset(ReceivedData, 0, MaxDataSizePerPacket); // Clear the receive Buffer(ReceivedDatafer)		
					ReceiveData(ClientSocket, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);

					if (!IsSuccessful)// When the size is 0
					{
						Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in IntialClientJoinToServerThreadIPv6TCP In: NetworkWrapper!");
						closesocket(ClientSocket);
					}
					else
					{
						ServerSideConnectionDisconnectionConfirmationTCP(ClientSocket, IsSuccessful, true, 2);
						if (!IsSuccessful)
						{
							closesocket(ClientSocket);
						}
						else
						{
							AddClientIPv6(&ClientSocket, &ClientAddress, IsSuccessful);//PENDING make it a Queue
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error IntialClientJoinToServerThreadIPv6TCP Failed In: NetworkWrapper!");
			}
		}

		//TCP Receive Data Loop Thread(After Succesful Join) 
		void ReceiveDataThreadTCPIPv4(ClientOrderIPv4* ClientInfo)
		{
			bool IsSuccessful = false;
			char* ReceivedData = (char*)malloc(MaxDataSizePerPacket);

			uint16_t Received_SizeOfData = 0;

			if (ReceivedData == nullptr)
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(MaxDataSizePerPacket) + " Byes Of Memory for ReceivedData in ReceiveDataThreadTCPIPv4 In: NetworkWrapper!\n");

				RemoveClientIPv4(ClientInfo->ClientNumber, IsSuccessful);//PENDING make it a queue
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv4 Failed in ReceiveDataThreadTCPIPv4 In: NetworkWrapper!");
				}
			}
			else
			{
				// Initial Check
				while (true)
				{
					memset(ReceivedData, 0, MaxDataSizePerPacket); // Clear the receive Buffer(ReceivedDatafer)
					ReceiveData(ClientInfo->ClientSocket, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
					while (!ContinueInputThread)//PENDING Set Atomic Boolean
					{
						//Infinite Loop here when ServerInputIsPaused
					}

					if (!IsSuccessful)// When the size is 0
					{
						Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveDataThreadTCPIPv4 In: NetworkWrapper!");
						RemoveClientIPv4(ClientInfo->ClientNumber, IsSuccessful);//PENDING make it a queue
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv4 Failed in ReceiveDataThreadTCPIPv4 In: NetworkWrapper!");
						}
						return;
					}
					else
					{
						CheckReceivedDataInfoServer(ReceivedData, false, Received_SizeOfData, IsSuccessful);
						if (IsSuccessful)
						{
							ClientInfo->AddReceivedPackage((ReceivedData + 18), Received_SizeOfData + 2, IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error ClientList::AddReceivedPackage Failed in ReceiveDataThreadTCPIPv4 In: NetworkWrapper!");
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed in ReceiveDataThreadTCPIPv4 In: NetworkWrapper!");
						}
					}
				}
			}
		}

		//TCP Receive Data Loop Thread(After Succesful Join) 
		void ReceiveDataThreadTCPIPv6(ClientOrderIPv6* ClientInfo)
		{
			bool IsSuccessful = false;
			char* ReceivedData = (char*)malloc(MaxDataSizePerPacket);

			uint16_t Received_SizeOfData = 0;

			if (ReceivedData == nullptr)
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(MaxDataSizePerPacket) + " Byes Of Memory for ReceivedData in ReceiveDataThreadTCPIPv6 In: NetworkWrapper!\n");

				RemoveClientIPv6(ClientInfo->ClientNumber, IsSuccessful);//PENDING make it a queue
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv6 Failed in ReceiveDataThreadTCPIPv6 In: NetworkWrapper!");
				}
			}
			else
			{
				// Initial Check
				while (true)
				{
					memset(ReceivedData, 0, MaxDataSizePerPacket); // Clear the receive Buffer(ReceivedDatafer)
					ReceiveData(ClientInfo->ClientSocket, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
					while (!ContinueInputThread)//PENDING Set Atomic Boolean
					{
						//Infinite Loop here when ServerInputIsPaused
					}

					if (!IsSuccessful)// When the size is 0
					{
						Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveDataThreadTCPIPv6 In: NetworkWrapper!");
						RemoveClientIPv6(ClientInfo->ClientNumber, IsSuccessful);//PENDING make it a queue
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv6 Failed in ReceiveDataThreadTCPIPv6 In: NetworkWrapper!");
						}
						return;
					}
					else
					{
						CheckReceivedDataInfoServer(ReceivedData, false, Received_SizeOfData, IsSuccessful);
						if (IsSuccessful)
						{
							ClientInfo->AddReceivedPackage((ReceivedData + 18), Received_SizeOfData + 2, IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error ClientList::AddReceivedPackage Failed in ReceiveDataThreadTCPIPv6 In: NetworkWrapper!");
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed in ReceiveDataThreadTCPIPv6 In: NetworkWrapper!");
						}
					}
				}
			}
		}

		//This Thread will be Run by Default for TCP And Will Close when Struct Destructs
		void ListenForTCPConnectionIPv4()
		{
			bool IsSuccessful = true;
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ListenForTCPConnectionIPv4 Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (!IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Listen For Incomming Client from A Client in ListenForTCPConnectionIPv4 In: NetworkWrapper!\n");
					Essenbp::WriteLogToFile("NOTE: Construct a Server To Send Data to Connected Clients\n");
				}
				else
				{
					SOCKET ClientSocket;
					sockaddr_in ClientAddress;
					int ClientAdderStructLength = sizeof(ClientAddress);
					while (true)
					{
						// Wait for message
						listen(ServerSocketIPv4, MaximumBackLogConnectionsTCP);
						ClientSocket = accept(ServerSocketIPv4, (sockaddr*)&ClientAddress, &ClientAdderStructLength);

						if (WSAGetLastError() > 0)
						{
							Essenbp::WriteLogToFile("\n Error accept() Failed with Error " + std::to_string(WSAGetLastError()) + " in ListenForTCPConnectionIPv4 In: NetworkWrapper!");
							continue;
						}

						std::thread(&IntialClientJoinToServerThreadIPv4TCP, ClientSocket, ClientAddress).detach();// Seperate Independent Thread No Outside Connection
					}
				}				
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ListenForTCPConnectionIPv4 Failed In: NetworkWrapper!");
			}
		}

		//TCP Receive Data Loop Thread(After Succesful Join) 
		void ListenForTCPConnectionIPv6()
		{
			bool IsSuccessful = true;
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ListenForTCPConnectionIPv6 Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (!IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying to Listen For Incomming Client from A Client in ListenForTCPConnectionIPv6 In: NetworkWrapper!\n");
					Essenbp::WriteLogToFile("NOTE: Construct a Server To Send Data to Connected Clients\n");
				}
				else
				{
					SOCKET ClientSocket;
					sockaddr_in6 ClientAddress;
					int ClientAdderStructLength = sizeof(ClientAddress);
					while (true)
					{
						// Wait for message
						listen(ServerSocketIPv6, MaximumBackLogConnectionsTCP);
						ClientSocket = accept(ServerSocketIPv6, (sockaddr*)&ClientAddress, &ClientAdderStructLength);

						if (WSAGetLastError() > 0)
						{
							Essenbp::WriteLogToFile("\n Error accept() Failed with Error " + std::to_string(WSAGetLastError()) + " in ListenForTCPConnectionIPv6 In: NetworkWrapper!");
							continue;
						}

						std::thread(&IntialClientJoinToServerThreadIPv6TCP, ClientSocket, ClientAddress).detach();// Seperate Independent Thread No Outside Connection
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ListenForTCPConnectionIPv6 Failed In: NetworkWrapper!");
			}
		}	

		//PENDING copy paste this for IPv6 
		void ReceiveDataThreadUDPIPv4()
		{
			bool IsSuccessful = false;
			char* ReceivedData = (char*)malloc(MaxDataSizePerPacket);

			uint16_t Received_SizeOfData = 0;

			if (ReceivedData == nullptr)
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(MaxDataSizePerPacket) + " Byes Of Memory for ReceivedData in ReceiveDataThreadUDPIPv4 In: NetworkWrapper!\n");
			}
			else
			{
				// Initial Check
				while (true)
				{
					memset(ReceivedData, 0, MaxDataSizePerPacket); // Clear the receive Buffer(ReceivedDatafer)
					ReceiveData(UDPServerSocketIPv4, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
					while (!ContinueInputThread)//PENDING Set Atomic Boolean
					{
						//Infinite Loop here when ServerInputIsPaused
					}

					if (!IsSuccessful)// When the size is 0
					{
						Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveDataThreadUDPIPv4 In: NetworkWrapper!");
					}
					else
					{
						CheckReceivedDataInfoServer(ReceivedData, false, Received_SizeOfData, IsSuccessful);
						if (IsSuccessful)
						{
							ClientInfo->AddReceivedPackage((ReceivedData + 18), Received_SizeOfData + 2, IsSuccessful);
							if (!IsSuccessful)
							{
								Essenbp::WriteLogToFile("\n Error ClientList::AddReceivedPackage Failed in ReceiveDataThreadUDPIPv4 In: NetworkWrapper!");
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed in ReceiveDataThreadUDPIPv4 In: NetworkWrapper!");
						}
					}
				}
			}
		}

		//PENDING make atomic Bool and while loop for this
		//NOTE: Run Two Instances of this One for IPv4 Other for IPv6
		void ReceiveDataTCPUDPForClient(char*ReceivedData, bool TrueForIPv6FalseForIPv4, bool& IsSuccessful)
		{
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling ReceiveDataTCPUDPForClient Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (IsServer)
				{
					Essenbp::WriteLogToFile("\n Error Trying Use Client Side Version from Server in ReceiveDataTCPUDPForClient In: NetworkWrapper!\n");
				}
				else
				{
					uint16_t Received_SizeOfData;
					if (IsClientTrueForTCPFalseForUDP)
					{
						if (TrueForIPv6FalseForIPv4)
						{
							ReceiveData(TCPServerSocketIPv6, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
						}
						else
						{
							ReceiveData(TCPServerSocketIPv4, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
						}
					}
					else
					{
						if (TrueForIPv6FalseForIPv4)
						{
							ReceiveData(UDPServerSocketIPv6, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
						}
						else
						{
							ReceiveData(UDPServerSocketIPv4, ReceivedData, MaxDataSizePerPacket, Received_SizeOfData, IsSuccessful);
						}
					}
					if (!IsSuccessful)// When the size is 0
					{
						Essenbp::WriteLogToFile("Error No Data Received From ReceiveData() in ReceiveDataTCPUDPForClient In: NetworkWrapper!");
					}
					else
					{
						ClientAddReceivedPackage(ReceivedData, Received_SizeOfData, IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("Error ClientAddReceivedPackage() Failed in ReceiveDataTCPUDPForClient In: NetworkWrapper!");
						}
					}
				}
			}
		}

		//PENDING
		void ProcessReceivedData()//PENDING Complete this for Each Client
		{

		}

		void NetworkWrapperMainThread()
		{
			//PENDING
		}

		public:
		//Start Thread //PENDING
		//Stop Thread  //PENDING
			
		//NOTE: If IPv4/IPv6 Is Present And If IPv6/IPv4 is also needed then Run this function
		void CreateSocket(std::string ServerIPAddress, unsigned int PortNumber, bool TrueForTCPFalseForUDP, bool TrueForIPv6FalseForIPv4, bool& IsSuccessful, std::string ClientIPAddress = "INADDR_ANY")
		{
			IsSuccessful = true;
			SOCKET* Socketptr = nullptr;
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling CreateSocket Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (TrueForTCPFalseForUDP)
				{
					if (TrueForIPv6FalseForIPv4)
					{
						Socketptr = &TCPServerSocketIPv6;
					}
					else
					{
						Socketptr = &TCPServerSocketIPv4;
					}
				}
				else
				{
					if (TrueForIPv6FalseForIPv4)
					{
						Socketptr = &UDPServerSocketIPv6;
					}
					else
					{
						Socketptr = &UDPServerSocketIPv4;
					}
				}

				if (Socketptr != NULL)
				{
					if (TrueForIPv6FalseForIPv4)
					{
						Essenbp::WriteLogToFile("\n Error IPv6 Socket Already Exists in CreateSocket In: NetworkWrapper!");
					}
					else
					{
						Essenbp::WriteLogToFile("\n Error IPv4 Socket Already Exists in CreateSocket In: NetworkWrapper!");
					}
					IsSuccessful = false;
				}				

				if (IsSuccessful)
				{
					if (TrueForTCPFalseForUDP)
					{
						if (TrueForIPv6FalseForIPv4)
						{
							*Socketptr = socket(PF_INET6, SOCK_STREAM, 0);
						}
						else
						{
							*Socketptr = socket(PF_INET, SOCK_STREAM, 0);
						}
					}
					else
					{
						if (TrueForIPv6FalseForIPv4)
						{
							*Socketptr = socket(PF_INET6, SOCK_DGRAM, 0);
						}
						else
						{
							*Socketptr = socket(PF_INET, SOCK_DGRAM, 0);
						}
					}

					if (*Socketptr == INVALID_SOCKET)
					{
						Essenbp::WriteLogToFile("\n Error socket() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
						Socketptr = nullptr;
						IsSuccessful = false;
					}

					if (IsSuccessful)
					{
						setsockopt(*Socketptr, SOL_SOCKET, SO_RCVTIMEO, (const char*)&InputTimeOut, sizeof(InputTimeOut));
						setsockopt(*Socketptr, SOL_SOCKET, SO_SNDTIMEO, (const char*)&OutputTimeOut, sizeof(OutputTimeOut));

						if (TrueForIPv6FalseForIPv4)
						{
							memset(&ServerHintIPv4, 0, sizeof(ServerHintIPv4));
							ServerHintIPv4.sin_family = PF_INET; // Address format is IPv4
							ServerHintIPv4.sin_port = htons(PortNumber); // Convert from little to big endian
							inet_pton(PF_INET, ServerIPAddress.c_str(), &(ServerHintIPv4.sin_addr));

							if (IsServer)
							{
								if (bind(*Socketptr, (sockaddr*)&ServerHintIPv4, sizeof(ServerHintIPv4)) == SOCKET_ERROR) // Bind The Socket to Ip And Port
								{
									Essenbp::WriteLogToFile("\n Error bind() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
									IsSuccessful = false;
								}
							}
							else
							{
								sockaddr_in ClientHintIPv4;
								memset(&ClientHintIPv4, 0, sizeof(ClientHintIPv4));
								ClientHintIPv4.sin_family = PF_INET; // Address format is IPv4
								ClientHintIPv4.sin_port = htons(0); // Convert from little to big endian
								if (ClientIPAddress == "INADDR_ANY")
								{
									ClientHintIPv4.sin_addr.s_addr = INADDR_ANY;
								}
								else
								{
									inet_pton(PF_INET, ClientIPAddress.c_str(), &(ClientHintIPv4.sin_addr));
								}

								if (bind(*Socketptr, (sockaddr*)&ClientHintIPv4, sizeof(ClientHintIPv4)) == SOCKET_ERROR) // Bind The Socket to Ip And Port
								{
									Essenbp::WriteLogToFile("\n Error bind() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
									IsSuccessful = false;
								}
								else
								{
									if (connect(*Socketptr, (sockaddr*)&ServerHintIPv4, sizeof(ServerHintIPv4)) == SOCKET_ERROR) // Connect The Socket to Ip And Port
									{
										Essenbp::WriteLogToFile("\n Error connet() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
										IsSuccessful = false;
									}
								}
							}
						}
						else
						{
							memset(&ServerHintIPv6, 0, sizeof(ServerHintIPv6));
							ServerHintIPv6.sin6_family = PF_INET6; // Address format is IPv6
							ServerHintIPv6.sin6_port = htons(PortNumber); // Convert from little to big endian
							inet_pton(PF_INET6, ServerIPAddress.c_str(), &(ServerHintIPv6.sin6_addr));

							if (IsServer)
							{
								if (bind(*Socketptr, (sockaddr*)&ServerHintIPv6, sizeof(ServerHintIPv6)) == SOCKET_ERROR) // Bind The Socket to Ip And Port
								{
									Essenbp::WriteLogToFile("\n Error bind() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
									IsSuccessful = false;
								}
							}
							else
							{
								sockaddr_in6 ClientHintIPv6;
								memset(&ClientHintIPv6, 0, sizeof(ClientHintIPv6));
								ClientHintIPv6.sin6_family = PF_INET6; // Address format is IPv4
								ClientHintIPv6.sin6_port = htons(0); // Convert from little to big endian
								if (ClientIPAddress == "INADDR_ANY")
								{
									ClientHintIPv6.sin6_addr = in6addr_any;
								}
								else
								{
									inet_pton(PF_INET6, ClientIPAddress.c_str(), &(ClientHintIPv6.sin6_addr));
								}

								if (bind(*Socketptr, (sockaddr*)&ClientHintIPv6, sizeof(ClientHintIPv6)) == SOCKET_ERROR) // Bind The Socket to Ip And Port
								{
									Essenbp::WriteLogToFile("\n Error bind() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
									IsSuccessful = false;
								}
								else
								{
									if (connect(*Socketptr, (sockaddr*)&ServerHintIPv6, sizeof(ServerHintIPv6)) == SOCKET_ERROR) // Connect The Socket to Ip And Port
									{
										Essenbp::WriteLogToFile("\n Error connet() Failed with Error " + std::to_string(WSAGetLastError()) + " in CreateSocket In: NetworkWrapper!");
										IsSuccessful = false;
									}
								}
							}
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error CreateSocket Failed In: NetworkWrapper!");
			}
		}

		//For Server And Client
		//NOTE: For Client Leave ProtocolSpecificaionTwo blank or set to None
		NetworkWrapper(std::string ServerIPAddress, unsigned int PortNumber, bool TrueForServerFalseForClient, bool& IsSuccessful, NetworkConstructorType ProtocolSpecificaionOne, NetworkConstructorType ProtocolSpecificaionTwo = NetworkConstructorType::None, std::string ClientIPAddress = "INADDR_ANY", int SocketInputTimeoutInSeconds = 60, int SocketOutputTimeoutInSeconds = 60, uint64_t MaximumUnusedClientSpots = 2048, uint16_t ArgSentPacketsArchiveSize = 128, uint16_t ArgReceivedPacketsArchiveSize = 128, int ArgMaximumBackLogConnectionsTCP_ONLY_FOR_TCP = 5, uint16_t ArgMaxDataSizePerPacket = 490) : IsServer(TrueForServerFalseForClient)
		{
			Essenbp::WriteLogToFile("\n Constructing NetworkWrapper!");

			if (!EndianCheckDone)
			{
				NW_PCheckIfLittleEndian();
			}

			TCPServerSocketIPv4 = NULL;
			TCPServerSocketIPv6 = NULL;
			UDPServerSocketIPv4 = NULL;
			UDPServerSocketIPv6 = NULL;

			ServerHintIPv4 = { 0 };
			ServerHintIPv6 = { 0 };

			IsConstructionSuccessful = false;
			IsSuccessful = true;

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
			//WORD version = MAKEWORD(2, 2);//Same below

			// Start WinSock
			int wsOk = WSAStartup(MAKEWORD(2, 2), &data);
			if (wsOk != 0)
			{
				Essenbp::WriteLogToFile("\n Error Winsock Startup Failed In: NetworkWrapper!");
				Essenbp::WriteLogToFile("\n Error Construction Failed NetworkWrapper!");
				IsSuccessful = false;
				return;
			}
			/*------------------------------------------------------------------------------------------------------------------*/
#endif			
			if (TrueForServerFalseForClient)
			{
				ClientsList = new ClientOrderList(MaximumUnusedClientSpots, SentPacketsArchiveSize, ReceivedPacketsArchiveSize);
				if (ClientsList == nullptr)
				{
					Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderList)) + " Byes Of Memory for ClientsList in AddClientIPv6 In: NetworkWrapper!\n");
					IsSuccessful = false;
				}
			}
			else
			{
				if (ProtocolSpecificaionTwo != NetworkConstructorType::None)
				{
					Essenbp::WriteLogToFile("\n Error ProtocolSpecificaionTwo Is Only For Server In NetworkWrapper!");
					IsSuccessful = false;
					delete ClientsList;
				}
				else
				{
					ClientSentPackets = new Essenbp::ArrayOfUnknownDataAndSize;
					if (ClientSentPackets != nullptr)
					{
						ClientReceivedPackets = new Essenbp::ArrayOfUnknownDataAndSize;
						if (ClientReceivedPackets == nullptr)
						{
							delete ClientSentPackets;
							IsSuccessful = false;
						}
					}
					else
					{
						IsSuccessful = false;
					}	
					delete ClientsList;
				}
			}

			if (IsSuccessful)
			{
				InputTimeOut = SocketInputTimeoutInSeconds * 1000;//60 Seconds InputTimeOut
				OutputTimeOut = SocketOutputTimeoutInSeconds * 1000;//60 Seconds OutputTimeOut
				MaxDataSizePerPacket = ArgMaxDataSizePerPacket;
				MaximumBackLogConnectionsTCP = ArgMaximumBackLogConnectionsTCP_ONLY_FOR_TCP;//Does not Matter for UDP
				SentPacketsArchiveSize = ArgSentPacketsArchiveSize;
				ReceivedPacketsArchiveSize = ArgReceivedPacketsArchiveSize;

				IsConstructionSuccessful = true;//Temp for the CreateSocket Function				
				
				IsSuccessful = false;
				switch (ProtocolSpecificaionOne)
				{
				case NW_P::NetworkConstructorType::TCPIPv4:
					CreateSocket(ServerIPAddress, PortNumber, true, false, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::TCPIPv6:
					CreateSocket(ServerIPAddress, PortNumber, true, true, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::UDPIPv4:
					CreateSocket(ServerIPAddress, PortNumber, false, false, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::UDPIPv6:
					CreateSocket(ServerIPAddress, PortNumber, false, true, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::TCPIPv4And6:
					CreateSocket(ServerIPAddress, PortNumber, true, false, IsSuccessful, ClientIPAddress);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error CreateSocket Failed In NetworkWrapper!");
					}
					else
					{
						CreateSocket(ServerIPAddress, PortNumber, true, true, IsSuccessful, ClientIPAddress);
					}					
					break;

				case NW_P::NetworkConstructorType::UDPIPv4And6:
					CreateSocket(ServerIPAddress, PortNumber, false, false, IsSuccessful, ClientIPAddress);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error CreateSocket Failed In NetworkWrapper!");
					}
					else
					{
						CreateSocket(ServerIPAddress, PortNumber, true, true, IsSuccessful, ClientIPAddress);
					}
					break;

				default:
					break;
				}	

				switch (ProtocolSpecificaionOne)
				{
				case NW_P::NetworkConstructorType::TCPIPv4:
					CreateSocket(ServerIPAddress, PortNumber, true, false, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::TCPIPv6:
					CreateSocket(ServerIPAddress, PortNumber, true, true, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::UDPIPv4:
					CreateSocket(ServerIPAddress, PortNumber, false, false, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::UDPIPv6:
					CreateSocket(ServerIPAddress, PortNumber, false, true, IsSuccessful, ClientIPAddress);
					break;

				case NW_P::NetworkConstructorType::TCPIPv4And6:
					CreateSocket(ServerIPAddress, PortNumber, true, false, IsSuccessful, ClientIPAddress);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error CreateSocket Failed In NetworkWrapper!");
					}
					else
					{
						CreateSocket(ServerIPAddress, PortNumber, true, true, IsSuccessful, ClientIPAddress);
					}
					break;

				case NW_P::NetworkConstructorType::UDPIPv4And6:
					CreateSocket(ServerIPAddress, PortNumber, false, false, IsSuccessful, ClientIPAddress);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error CreateSocket Failed In NetworkWrapper!");
					}
					else
					{
						CreateSocket(ServerIPAddress, PortNumber, true, true, IsSuccessful, ClientIPAddress);
					}
					break;

				case NW_P::NetworkConstructorType::None:
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error No Socket Type is Created In NetworkWrapper!");
					}
					break;

				default:
					break;
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetworkWrapper!");
				IsConstructionSuccessful = false;
				if (ClientsList != nullptr)
				{
					delete ClientsList;
				}
			}
			else
			{
				IsConstructionSuccessful = true;
			}
		}

		~NetworkWrapper()
		{
			Essenbp::WriteLogToFile("\n Destructing NetworkWrapper!");
			if (IsConstructionSuccessful)
			{
				if (TCPServerSocketIPv4 != NULL)
				{
					closesocket(TCPServerSocketIPv4);
				}
				if (TCPServerSocketIPv6 != NULL)
				{
					closesocket(TCPServerSocketIPv6);
				}
				if (UDPServerSocketIPv4 != NULL)
				{
					closesocket(UDPServerSocketIPv4);
				}
				if (UDPServerSocketIPv6 != NULL)
				{
					closesocket(UDPServerSocketIPv6);
				}
				if (ClientsList != nullptr)
				{
					delete ClientsList;
				}
				if (ClientSentPackets != nullptr)
				{
					delete ClientSentPackets;
				}
				if (ClientReceivedPackets != nullptr)
				{
					delete ClientReceivedPackets;
				}
				IsConstructionSuccessful = false;
			}
		}
	};
};


#endif // !NETWORKING_WRAPPER_BY_PUNAL