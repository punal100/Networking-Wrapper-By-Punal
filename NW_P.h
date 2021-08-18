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

	//NOTE: Similar to UnknownDataAndSizeStruct But the Data is Stored in Big Endian Format
	//NOTE: Pass Each Variable One by One like this CopyAndStoreData(Data, SizeOfData, Issuccessful, false, AppendData = true)
	//NOTE: If Each Variable Is Not Passed One by One, Then Expect Undefined Behaviours...
	struct NetworkDataAndSizeStruct
	{
	private:
		char* Data = nullptr;
		size_t SizeOfData = 0;

	public:
		NetworkDataAndSizeStruct()
		{
			Essenbp::WriteLogToFile("\n Constructing NetworkDataAndSizeStruct!");
			if (!EndianCheckDone)
			{
				NW_PCheckIfLittleEndian();
			}
		}

		void FreeData()
		{
			if (Data != nullptr)
			{
				free(Data);//Free Previous Data
				Data = nullptr;
				SizeOfData = 0;
			}
		}

		//NOTE DoADummyCopy stores only the Size but not the data
		void CopyAndStoreData(void* ArgData, size_t ArgSizeOfData, bool& Issuccessful, bool DoADummyCopy = false, bool AppendData = false)
		{
			Issuccessful = false;

			if (!DoADummyCopy)
			{
				if (ArgData == nullptr)
				{
					Essenbp::WriteLogToFile("\n Error nullptr for ArgData in CopyAndStoreData In: NetworkDataAndSizeStruct!\n");
					Essenbp::WriteLogToFile("If Dummy Value is to be passed set DoADummyCopy(4th argument) to false, and set ArgData = nullptr!\n");
				}
				else
				{
					if (AppendData)
					{
						if (ArgSizeOfData == 0)
						{
							Essenbp::WriteLogToFile("\n Error Size Of ArgSizeOfData is Equal to Zero in CopyAndStoreData In: NetworkDataAndSizeStruct!\n");
							return;
						}

						char* AppendDataHelper = (char*)malloc((SizeOfData + ArgSizeOfData));// Setting Current
						if (AppendDataHelper == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating : " + std::to_string(SizeOfData + ArgSizeOfData) + " Byes Of Memory for Data in CopyAndStoreData In: NetworkDataAndSizeStruct!\n");
							return;
						}
						else
						{
							//PENDING CHECK FOR ERROR
							size_t PreviousSize = SizeOfData;
							SizeOfData = SizeOfData + ArgSizeOfData;// Current Size

							for (size_t i = 0; i < PreviousSize; ++i)// Memccpy bad
							{
								AppendDataHelper[i] = ((char*)Data)[i];// I could simply convert void* to char*... but i left it as void* for the purpose of 'readability'
							}

							if (IsLittleEndian)
							{								
								if (ArgSizeOfData != 0)
								{
									size_t i = (ArgSizeOfData - 1);
									while (true)
									{
										//Visual Studio 2019 Is saying Buffer Overrun by 2 bytes for AppendDatHelper, the writable size is only (SizeOfData + ArgSizeOfData)
										//But Evrything is Correct here? Buffer overrun is impossible!
										// I could simply convert void* to char*... but i left it as void* for the purpose of 'readability'
										AppendDataHelper[(i + PreviousSize)] = ((char*)ArgData)[i];
										if (i == 0)
										{
											break;
										}
										else
										{
											i = i - 1;
										}
									}
								}
							}
							else
							{
								for (size_t i = 0; i < ArgSizeOfData; ++i)// Memccpy bad
								{
									//Visual Studio 2019 Is saying Buffer Overrun by 2 bytes for AppendDatHelper, the writable size is only (SizeOfData + ArgSizeOfData)
									//But Evrything is Correct here? Buffer overrun is impossible!
									AppendDataHelper[(i + PreviousSize)] = ((char*)ArgData)[i];// I could simply convert void* to char*... but i left it as void* for the purpose of 'readability'
								}
							}

							FreeData();
							Data = AppendDataHelper;
							Issuccessful = true;
						}
					}
					else
					{
						if (ArgSizeOfData == 0)
						{
							Essenbp::WriteLogToFile("\n Error Size Of SizeOfData is Equal to Zero in CopyAndStoreData In: NetworkDataAndSizeStruct!\n");
							return;
						}
						FreeData();
						SizeOfData = ArgSizeOfData;
						Data = (char*)malloc(SizeOfData);
						if (Data == nullptr)
						{
							SizeOfData = 0;
							Essenbp::WriteLogToFile("\n Error Allocating : " + std::to_string(SizeOfData) + " Byes Of Memory for Data in CopyAndStoreData In: NetworkDataAndSizeStruct!\n");
							return;
						}
						else
						{
							if (IsLittleEndian)
							{
								if (ArgSizeOfData != 0)
								{
									size_t i = (ArgSizeOfData - 1);
									while (true)
									{
										//Visual Studio 2019 Is saying Buffer Overrun by 2 bytes for AppendDatHelper, the writable size is only (SizeOfData + ArgSizeOfData)
										//But Evrything is Correct here? Buffer overrun is impossible!
										// I could simply convert void* to char*... but i left it as void* for the purpose of 'readability'
										Data[i] = ((char*)ArgData)[i];
										if (i == 0)
										{
											break;
										}
										else
										{
											i = i - 1;
										}
									}
								}
							}
							else
							{
								for (size_t i = 0; i < ArgSizeOfData; ++i)// Memccpy bad
								{
									//Visual Studio 2019 Is saying Buffer Overrun by 2 bytes for AppendDatHelper, the writable size is only (SizeOfData + ArgSizeOfData)
									//But Evrything is Correct here? Buffer overrun is impossible!
									Data[i] = ((char*)ArgData)[i];// I could simply convert void* to char*... but i left it as void* for the purpose of 'readability'
								}
							}
							Issuccessful = true;
						}
					}
				}
			}
			else
			{
				if (ArgData != nullptr)
				{
					Essenbp::WriteLogToFile("\n Error ArgData is not set to nullptr in CopyAndStoreData In: NetworkDataAndSizeStruct!\n");
					Essenbp::WriteLogToFile("If Actual value is to be passed ignore DoADummyCopy(4th argument)!\n");
					return;
				}
				else
				{
					FreeData();
					SizeOfData = ArgSizeOfData;
					Issuccessful = true;
				}
			}
		}

		char* GetData() { return Data; }
		size_t GetDataSize() { return SizeOfData; }

		//NOTE: Use this if Direct access to data is required
		//NOTE: by using this
		void FreeAndResizeData(size_t ArgSizeOfData, bool& Issuccessful)
		{
			Issuccessful = false;

			FreeData();
			if (ArgSizeOfData == 0)
			{
				Essenbp::WriteLogToFile("\n Error Size Of SizeOfData is Equal to Zero in FreeAndResizeDataAndReturnPointerToDataPointer In: NetworkDataAndSizeStruct!\n");
				return;
			}
			Data = (char*)malloc(ArgSizeOfData);
			if (Data == nullptr)
			{
				Essenbp::WriteLogToFile("\n Error Allocating : " + std::to_string(SizeOfData) + " Byes Of Memory for Data in FreeAndResizeDataAndReturnPointerToDataPointer In: NetworkDataAndSizeStruct!\n");
				return;
			}
			SizeOfData = ArgSizeOfData;
			Issuccessful = true;
		}

		//NOTE: The Data Is Returned But, not released, SO USE THIS WITH CAUTION, if being careless then there is a HUGE chance of memory leak
		void GetDataAndSizeAndSetDataToNull(void** ReturnData, size_t& ReturnDataSize)
		{
			if (ReturnData != nullptr)
			{
				*ReturnData = Data;
				ReturnDataSize = SizeOfData;
				Data = nullptr;
				SizeOfData = 0;
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error Argument ReturnData is nullptr in GetDataAndSizeAndSetDataToNull In: NetworkDataAndSizeStruct!\n");
			}
		}

		//NOTE: Data Starts from 0
		//NOTE: Max Size is From = 0, To = SizeOfData - 1
		void ReverseDataOrder(size_t From, size_t To, bool& IsSuccessful)
		{
			IsSuccessful = false;
			if (To < SizeOfData)
			{
				Essenbp::ReverseDataOrder((void*)Data, From, To, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Essenbp::ReverseDataOrder in in ReverseDataOrder In: NetworkDataAndSizeStruct!\n");
				}
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error To Is greater than SizeOfData in ReverseDataOrder In: NetworkDataAndSizeStruct!\n");
			}
		}

		~NetworkDataAndSizeStruct()
		{
			FreeData();
		}
	};

	struct ArrayOfNetworkDataAndSize
	{
	private:
		unsigned int TotalNumberOfNetworkData = 0;
		NetworkDataAndSizeStruct** ArrayOfNetworkData = nullptr;

	public:
		void ResizeArray(unsigned int TotalNumber, bool& Issuccessful)
		{
			Issuccessful = false;

			NetworkDataAndSizeStruct** TempNetworkData = nullptr;
			Essenbp::Malloc_PointerToArrayOfPointers((void***)&TempNetworkData, TotalNumber, sizeof(NetworkDataAndSizeStruct*), Issuccessful);
			if (!Issuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Essenbp::Malloc_PointerToArrayOfPointers failed in ResizeArray In: ArrayOfNetworkDataAndSize!\n");
			}
			else
			{
				Issuccessful = true;
				int MaxLimit = (TotalNumber < TotalNumberOfNetworkData) ? TotalNumber : TotalNumberOfNetworkData;

				for (int i = 0; i < MaxLimit; ++i)
				{
					TempNetworkData[i] = ArrayOfNetworkData[i];
				}
				if (TotalNumber > TotalNumberOfNetworkData)
				{
					for (int i = TotalNumberOfNetworkData; i < TotalNumber; ++i)
					{
						TempNetworkData[i] = new NetworkDataAndSizeStruct();
						if (TempNetworkData[i] == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating Bytes of Data for NetworkDataAndSizeStruct[" + std::to_string(i) + "] in ResizeArray In: ArrayOfNetworkDataAndSize!\n");
							for (int j = 0; j < i; ++j)
							{
								delete TempNetworkData[j];
							}
							free(TempNetworkData);
							Issuccessful = false;
							break;
						}
					}
				}
				else
				{
					for (int j = TotalNumber; j < TotalNumberOfNetworkData; ++j)
					{
						delete ArrayOfNetworkData[j];
					}
				}

				if (!Issuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ResizeArray failed In: ArrayOfNetworkDataAndSize!\n");
				}
				else
				{
					ArrayOfNetworkData = TempNetworkData;
					TotalNumberOfNetworkData = TotalNumber;
				}
			}
		}
	
		ArrayOfNetworkDataAndSize()
		{
			Essenbp::WriteLogToFile("\n Constructing ArrayOfNetworkDataAndSize!");
			if (!EndianCheckDone)
			{
				NW_PCheckIfLittleEndian();
			}
		}

		void AddElement(bool& Issuccessful)
		{
			Issuccessful = false;
			ResizeArray(TotalNumberOfNetworkData + 1, Issuccessful);
			if (!Issuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ResizeArray failed in AddElement In: ArrayOfNetworkDataAndSize!\n");
			}
		}

		void RemoveElement(unsigned int ElementNumber, bool& Issuccessful)
		{
			Issuccessful = false;
			if (ElementNumber > TotalNumberOfNetworkData)
			{
				Essenbp::WriteLogToFile("\n Error ElementNumber Exceeds the total number of Network Data Present! in RemoveElement in AddElement In: ArrayOfNetworkDataAndSize!\n");
			}
			else
			{
				if (TotalNumberOfNetworkData == 1)
				{
					delete ArrayOfNetworkData[0];
				}
				else
				{
					//The Gap is filled by next element, and the next element's gap is filled by the element next to it, and so on until last element
					//Example 1,2,3,4,5
					//Remove 3
					//1,2, gap ,4,5
					//4 fills 3rd gap
					//1,2,4, gap ,5
					//5 fills 4rd gap
					//1,2,4,5, gap
					//1,2,4,5 (gap is deleted)
					NetworkDataAndSizeStruct* TempChangeptr = ArrayOfNetworkData[ElementNumber];

					for (int i = TotalNumberOfNetworkData - 1; i > ElementNumber; --i)
					{
						ArrayOfNetworkData[(i - 1)] = ArrayOfNetworkData[i];
					}
					ArrayOfNetworkData[TotalNumberOfNetworkData - 1] = TempChangeptr;

					ResizeArray(TotalNumberOfNetworkData - 1, Issuccessful);
					if (!Issuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ResizeArray failed in RemoveElement In: ArrayOfNetworkDataAndSize!\n");
					}
				}

			}
		}

		unsigned int GetTotalNumberOfNetworkData()
		{
			return TotalNumberOfNetworkData;
		}

		void GetData(unsigned int ElementNumber, NetworkDataAndSizeStruct** ReturnNetworkDataAndSize, bool& Issuccessful)
		{
			if (ElementNumber < TotalNumberOfNetworkData)
			{
				*ReturnNetworkDataAndSize = ArrayOfNetworkData[ElementNumber];
			}
			else
			{
				Essenbp::WriteLogToFile("\n Error ElementNumber Exceeds the total number of Network Data Present! in GetData in AddElement In: ArrayOfNetworkDataAndSize!\n");
			}
		}

		~ArrayOfNetworkDataAndSize()
		{
			for (int j = 0; j < TotalNumberOfNetworkData; ++j)
			{
				delete ArrayOfNetworkData[j];
			}
			free(ArrayOfNetworkData);
		}
	};	

	//NOTE: Use NetAddr Struct Instead of NetAddrIPv4/6
	struct NetAddrIPv4
	{
	public:
		uint64_t ClientUniqueID = 0;//This should be changed Every Specified Minutes For Security Purposes...

		const SOCKET Socket;
		const uint64_t UniqueNumber;//Number = 0 => Server, Number > 0 => Client //PENDING Modify this with functions
		const sockaddr_in NetAddress;
		const uint16_t SentPacketsArchiveSize;
		const uint16_t ReceivedPacketsArchiveSize;

		ArrayOfNetworkDataAndSize SentPackets;
		ArrayOfNetworkDataAndSize ReceivedPackets;
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
				Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::ResizeArray for SentPackets Failed NetAddrIPv4!");
			}
			else
			{
				ReceivedPackets.ResizeArray(ReceivedPacketsArchiveSize, IsConstructionSuccessful);
				if (!IsConstructionSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::ResizeArray for ReceivedPackets Failed NetAddrIPv4!");
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
				NetworkDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				SentPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::GetData Failed in AddSentPackage In: NetAddrIPv4!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error NetworkDataAndSizeStruct::CopyAndStoreData Failed in AddSentPackage In: NetAddrIPv4!");
					}
					else
					{
						SentCount = SentCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddSentPackage() In: NetAddrIPv4!");
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
				NetworkDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				ReceivedPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::GetData Failed in AddReceivedPackage In: NetAddrIPv4!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error NetworkDataAndSizeStruct::CopyAndStoreData Failed in AddReceivedPackage In: NetAddrIPv4!");
					}
					else
					{
						ReceivedCount = ReceivedCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddSentPackage() In: NetAddrIPv4!");
			}
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

		const SOCKET Socket;
		const uint64_t UniqueNumber;//Number = 0 => Server, Number > 0 => Client //PENDING Modify this with functions
		const sockaddr_in6 NetAddress;
		const uint16_t SentPacketsArchiveSize;
		const uint16_t ReceivedPacketsArchiveSize;

		ArrayOfNetworkDataAndSize SentPackets;
		ArrayOfNetworkDataAndSize ReceivedPackets;
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
				Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::ResizeArray for SentPackets Failed NetAddrIPv6!");
			}
			else
			{
				ReceivedPackets.ResizeArray(ReceivedPacketsArchiveSize, IsConstructionSuccessful);
				if (!IsConstructionSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::ResizeArray for ReceivedPackets Failed NetAddrIPv6!");
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddrIPv6!");
			}
		}

		void AddSentPackage(char* Data, size_t DataSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddSentPackage Without Constructing the struct In: NetAddrIPv6!\n");
			}
			else
			{
				NetworkDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				SentPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::GetData Failed in AddSentPackage In: NetAddrIPv6!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error NetworkDataAndSizeStruct::CopyAndStoreData Failed in AddSentPackage In: NetAddrIPv6!");
					}
					else
					{
						SentCount = SentCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddSentPackage() In: NetAddrIPv6!");
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
				NetworkDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
				ReceivedPackets.GetData(SentCount, &PtrToDataAndDataSize, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::GetData Failed in AddReceivedPackage In: NetAddrIPv6!");
				}
				else
				{
					PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error NetworkDataAndSizeStruct::CopyAndStoreData Failed in AddReceivedPackage In: NetAddrIPv6!");
					}
					else
					{
						ReceivedCount = ReceivedCount + 1;
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddSentPackage() In: NetAddrIPv6!");
			}
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
	struct NetAddr
	{
	public:
		const bool TrueForIPv6FalseForIPv4;
		NetAddrIPv4* IPv4Addr = nullptr;
		NetAddrIPv6* IPv6Addr = nullptr;

		bool IsConstructionSuccessful;

		NetAddr(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize) : TrueForIPv6FalseForIPv4(false)
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddr!");

			IsConstructionSuccessful = false;

			IPv4Addr = new NetAddrIPv4(ArgSocket, ArgUniqueNumber, ArgNetAddress, ArgSentPacketsArchiveSize, ArgReceivedPacketsArchiveSize);

			if (IPv4Addr != nullptr)
			{
				if (IPv4Addr->IsConstructionSuccessful)
				{
					IsConstructionSuccessful = true;
				}
				else
				{
					delete IPv4Addr;
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddr!");
			}
		}

		NetAddr(SOCKET ArgSocket, uint64_t ArgUniqueNumber, sockaddr_in6 ArgNetAddress, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize) : TrueForIPv6FalseForIPv4(true)
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddr!");

			IsConstructionSuccessful = false;

			IPv6Addr = new NetAddrIPv6(ArgSocket, ArgUniqueNumber, ArgNetAddress, ArgSentPacketsArchiveSize, ArgReceivedPacketsArchiveSize);

			if (IPv6Addr != nullptr)
			{
				if (IPv6Addr->IsConstructionSuccessful)
				{
					IsConstructionSuccessful = true;
				}
				else
				{
					delete IPv6Addr;
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddr!");
			}
		}

		~NetAddr()
		{
			Essenbp::WriteLogToFile("\n Destructing NetAddr!");
			if (IsConstructionSuccessful)
			{				
				if (IPv4Addr != nullptr)
				{
					delete IPv4Addr;
				
				}
				if (IPv6Addr != nullptr)
				{
					delete IPv6Addr;
				}

				IPv4Addr = nullptr;
				IPv6Addr = nullptr;
				IsConstructionSuccessful = false;
			}
		}
	};

	//NOTE: A List of NetAddar(With const values for Construction)
	//NOTE: MinmumFreeSpotsInArray Means At a time how many free spots can be reserved	
	//NOTE: TotalNumberOfNetAddr Maximum Value is A Multiple of MinmumFreeSpotsInArray	
	//NOTE: Array Number of 0 Is Reserved, So Array Number Starts from 1 to TotalNumberOfNetAddr(== (n * MinmumFreeSpotsInArray) + 1)
	//NOTE: Lets say Initially TotalNumberOfNetAddr = (1 * MinmumFreeSpotsInArray) + 1 Once every Spots is Used
	//NOTE: It will again be Filled with more free SpotsTotalNumberOfNetAddr = (2 * MinmumFreeSpotsInArray) + 1 and So on until buffer overflow...
	//NOTE: TotalNumberOfFreeNetAddarSpotsInArray is >= _RENAMEThisTEMPNAME_ the Array is Reordered
	struct NetAddrArray
	{
	private:
		NetAddr** ArrayOfNetAddr = nullptr;
		uint64_t TotalNumberOfNetAddr = 0;

		//PENDING _RENAMEThisTEMPNAME_ Should be changed...
		//PENDING make Variable for Removed Numbers which is beyond reserved number like 2048 + n(== > 0)
		//PENDING check MinumumFreeSpots And Maximum Free Spots
		const uint64_t MinmumFreeSpotsInArray = 0;//Default is 2048, Every Multiple of MinmumFreeSpotsInArray == n * MinmumFreeSpotsInArray the Array Is Reordered Only when Increasing
		//const uint64_t _RENAMEThisTEMPNAME_ = 0;//Default is 4096, if TotalNumberOfFreeNetAddarSpotsInArray >= _RENAMEThisTEMPNAME_ The Array Is Reordered
		const uint16_t SentPacketsArchiveSize;
		const uint16_t ReceivedPacketsArchiveSize;

		//uint64_t* UnusedClientpotIPv4 = nullptr;
		uint64_t* FreeNetAddarSpotsInArray = nullptr;
		uint64_t TotalNumberOfFreeNetAddarSpotsInArray = 0;//TotalNumberOfFreeNetAddrSpotsInArray = MinmumFreeSpotsInArray + 1// +1 is because 0th element is reserved...
		//uint64_t TotalNumberOfUnusedClientSpotIPv6 = 0;

		bool IsConstructionSuccessful = false;

		//PENDING make Removing 0th Element Impossible
		//PENDING make Retreiving 0th Element Impossible
		//PENDING 0th Element set it to server or something

	public:
		NetAddrArray(uint64_t ArgMinmumFreeSpotsInArray, uint16_t ArgSentPacketsArchiveSize, uint16_t ArgReceivedPacketsArchiveSize) : MinmumFreeSpotsInArray(ArgMinmumFreeSpotsInArray), SentPacketsArchiveSize(ArgSentPacketsArchiveSize), ReceivedPacketsArchiveSize(ArgSentPacketsArchiveSize)
		{
			Essenbp::WriteLogToFile("\n Constructing NetAddrArray!");

			ArrayOfNetAddr = nullptr;
			TotalNumberOfNetAddr = 0;

			FreeNetAddarSpotsInArray = nullptr;
			TotalNumberOfFreeNetAddarSpotsInArray = 0;

			if (_RENAMEThisTEMPNAME_ < MinmumFreeSpotsInArray)
			{
				Essenbp::WriteLogToFile("\n Error _RENAMEThisTEMPNAME_ is Less than MinmumFreeSpotsInArray In: NetAddrArray!\n");
			}
			else
			{
				FreeNetAddarSpotsInArray = (uint64_t*)calloc((MinmumFreeSpotsInArray + 1), sizeof(uint64_t));
				if (FreeNetAddarSpotsInArray == nullptr)
				{
					Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((MinmumFreeSpotsInArray + 1) * sizeof(uint64_t)) + " Byes Of Memory for FreeNetAddarSpotsInArray In: NetAddrArray!\n");
				}
				else
				{
					Essenbp::Malloc_PointerToArrayOfPointers((void***)&ArrayOfNetAddr, (MinmumFreeSpotsInArray + 1), sizeof(NetAddr*), IsConstructionSuccessful);
					if (!IsConstructionSuccessful)
					{
						free(FreeNetAddarSpotsInArray);
						Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((MinmumFreeSpotsInArray + 1) * sizeof(NetAddr*)) + " Byes Of Memory for ArrayOfNetAddr In: NetAddrArray!\n");
					}
					else
					{
						FreeNetAddarSpotsInArray[0] = 0;
						for (uint64_t i = 1; i < (MinmumFreeSpotsInArray + 1); ++i)
						{
							FreeNetAddarSpotsInArray[i] = (MinmumFreeSpotsInArray + 1) - i;
							ArrayOfNetAddr[i] = nullptr;
						}
						TotalNumberOfFreeNetAddarSpotsInArray = MinmumFreeSpotsInArray + 1;
					}
				}
			}

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetAddrArray!");
			}
		}

		//PENDING add make Variable for Removed Numbers which is beyond reserved number like 2048 + n(== > 0)
		void AddNetAddr(SOCKET Socket, sockaddr_in Address, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddNetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				//Spot Array Number 0 Is Reserved
				if (TotalNumberOfFreeNetAddarSpotsInArray > 1)
				{
					//ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] == Fills the First Unused Element
					ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] = new NetAddr(Socket, FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)], Address, SentPacketsArchiveSize, ReceivedPacketsArchiveSize);// NO need for IsSuccessful Constrction check as it is not needed in this simple struct
					if (ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] != nullptr)
					{
						IsSuccessful = false;
					}
					else
					{
						if (ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]]->IsConstructionSuccessful == false)
						{
							IsSuccessful = false;
							delete ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]];
							ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] = nullptr;
						}
						else
						{
							IsSuccessful = true;
							FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)] = 0;//Reseted
							TotalNumberOfFreeNetAddarSpotsInArray = TotalNumberOfFreeNetAddarSpotsInArray - 1;
							TotalNumberOfNetAddr = TotalNumberOfNetAddr + 1;
						}
					}
				}
				else
				{
					FreeNetAddarSpotsInArray = (uint64_t*)calloc((TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1), sizeof(uint64_t));
					if (FreeNetAddarSpotsInArray == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1) * sizeof(uint64_t)) + " Byes Of Memory for FreeNetAddarSpotsInArray In: NetAddrArray!\n");
					}
					else
					{
						NetAddr** TEMPArrayOfNetAddr = nullptr;
						Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPArrayOfNetAddr, (TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1), sizeof(NetAddr*), IsConstructionSuccessful);
						if (!IsConstructionSuccessful)
						{
							free(FreeNetAddarSpotsInArray);
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1) * sizeof(NetAddr*)) + " Byes Of Memory for TEMPArrayOfNetAddr In: NetAddrArray!\n");
						}
						else
						{
							//This Copies Previous
							for (uint64_t i = 0; i < TotalNumberOfNetAddr + 1; ++i)
							{
								TEMPArrayOfNetAddr[i] = ArrayOfNetAddr[i];
							}

							FreeNetAddarSpotsInArray[0] = 0;
							for (uint64_t i = 1; i < MinmumFreeSpotsInArray + 1; ++i)
							{
								FreeNetAddarSpotsInArray[i] = (TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1) - i;
							}

							//Sets nullptr for Allocated Space
							for (uint64_t i = TotalNumberOfNetAddr + 1; i < (TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1); ++i)
							{
								TEMPArrayOfNetAddr[i] = nullptr;
							}

							TotalNumberOfFreeNetAddarSpotsInArray = MinmumFreeSpotsInArray + 1;
							free(ArrayOfNetAddr);
							ArrayOfNetAddr = TEMPArrayOfNetAddr;
						}
						if (IsSuccessful)
						{
							AddNetAddr(Socket, Address, IsSuccessful);//PENDING Check this Function Upon Testing
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddNetAddr() Failed in NetAddrArray!");
			}
		}

		void AddNetAddr(SOCKET Socket, sockaddr_in6 Address, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddNetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				//Spot Array Number 0 Is Reserved
				if (TotalNumberOfFreeNetAddarSpotsInArray > 1)
				{
					//ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] == Fills the First Unused Element
					ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] = new NetAddr(Socket, FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)], Address, SentPacketsArchiveSize, ReceivedPacketsArchiveSize);// NO need for IsSuccessful Constrction check as it is not needed in this simple struct
					if (ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] == nullptr)
					{
						IsSuccessful = false;
					}
					else
					{
						if (ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]]->IsConstructionSuccessful == false)
						{
							IsSuccessful = false;
							delete ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]];
							ArrayOfNetAddr[FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)]] = nullptr;
						}
						else
						{
							IsSuccessful = true;
							FreeNetAddarSpotsInArray[(TotalNumberOfFreeNetAddarSpotsInArray - 1)] = 0;//Reseted
							TotalNumberOfFreeNetAddarSpotsInArray = TotalNumberOfFreeNetAddarSpotsInArray - 1;
							TotalNumberOfNetAddr = TotalNumberOfNetAddr + 1;
						}
					}
				}
				else
				{
					FreeNetAddarSpotsInArray = (uint64_t*)calloc((TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1), sizeof(uint64_t));
					if (FreeNetAddarSpotsInArray == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1) * sizeof(uint64_t)) + " Byes Of Memory for FreeNetAddarSpotsInArray In: NetAddrArray!\n");
					}
					else
					{
						NetAddr** TEMPArrayOfNetAddr = nullptr;
						Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPArrayOfNetAddr, (TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1), sizeof(NetAddr*), IsConstructionSuccessful);
						if (!IsConstructionSuccessful)
						{
							free(FreeNetAddarSpotsInArray);
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1) * sizeof(NetAddr*)) + " Byes Of Memory for TEMPArrayOfNetAddr In: NetAddrArray!\n");
						}
						else
						{
							//This Copies Previous
							for (uint64_t i = 0; i < TotalNumberOfNetAddr + 1; ++i)
							{
								TEMPArrayOfNetAddr[i] = ArrayOfNetAddr[i];
							}

							FreeNetAddarSpotsInArray[0] = 0;
							for (uint64_t i = 1; i < MinmumFreeSpotsInArray + 1; ++i)
							{
								FreeNetAddarSpotsInArray[i] = (TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1) - i;
							}

							//Sets nullptr for Allocated Space
							for (uint64_t i = TotalNumberOfNetAddr + 1; i < (TotalNumberOfNetAddr + MinmumFreeSpotsInArray + 1); ++i)
							{
								TEMPArrayOfNetAddr[i] = nullptr;
							}

							TotalNumberOfFreeNetAddarSpotsInArray = MinmumFreeSpotsInArray + 1;
							free(ArrayOfNetAddr);
							ArrayOfNetAddr = TEMPArrayOfNetAddr;
						}
						if (IsSuccessful)
						{
							AddNetAddr(Socket, Address, IsSuccessful);//PENDING Check this Function Upon Testing
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error AddNetAddr() Failed in NetAddrArray!");
			}
		}

		//PENDING COMPLETE THIS
		//NOTE: 0th Element Can't be removed it is reserved
		//NOTE: Only 1 to TotalNumberOfNetAddr(== (n * MinmumFreeSpotsInArray) + 1) can be removed
		void RemoveNetAddr(uint64_t ArrayNumber, bool& IsSuccessful)
		{
			IsSuccessful = false;
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveNetAddr Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (ArrayNumber <= TotalNumberOfNetAddr)
				{
					if (ArrayNumber == 0)
					{
						Essenbp::WriteLogToFile("\n Error Trying to Remove the Reserved Element Number 0 In RemoveNetAddr In: NetAddrArray!\n");
						IsSuccessful = false;
					}
					else
					{
						if (ArrayOfNetAddr[ArrayNumber] == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error The Element At ArrayNumberr " + std::to_string(ArrayNumber) + " Is Empty In RemoveNetAddr In: NetAddrArray!\n");
							IsSuccessful = false;
						}
						else
						{
							if (TotalNumberOfFreeNetAddarSpotsInArray >= _RENAMEThisTEMPNAME_)//PENDING WRONG _RENAMEThisTEMPNAME_ is Wrong Change it!
							{
								//PENDING COMPLETE THIS
							}
							else
							{
								ArrayOfNetAddr[ArrayNumber] == nullptr;
								TotalNumberOfFreeNetAddarSpotsInArray = TotalNumberOfFreeNetAddarSpotsInArray + 1;

								uint64_t i = ArrayNumber;
								i = i % _RENAMEThisTEMPNAME_;
								i = (i == 0) ? _RENAMEThisTEMPNAME_ : i;//if(i == 0){ i = _RENAMEThisTEMPNAME_; }else{ i = i; }

								for (i = i; i < MinmumFreeSpotsInArray + 1; ++i)
								{
									FreeNetAddarSpotsInArray[i] = TotalNumberOfNetAddr + MinmumFreeSpotsInArray - i;
								}
								FreeNetAddarSpotsInArray
								//PENDING COMPLETE THIS
							}
						}
					}
				}
			}
		}

		//PENDING change RemoveClientIPv4 And RemoveClientIPv4 to RemoveNetAddr
		void RemoveClientIPv4(uint64_t ClientNumber, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveClientIPv4 Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv4)
				{
					if (ArrayOfNetAddr[ClientNumber] == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ClientNumber " + std::to_string(ClientNumber) + " Is Already Removeded from ArrayOfNetAddr In RemoveClientIPv4 In: NetAddrArray!\n");
						IsSuccessful = false;
					}
					else
					{
						if (ArrayOfNetAddr[ClientNumber]->ClientSocket != NULL)
						{
							closesocket(ArrayOfNetAddr[ClientNumber]->ClientSocket);
						}
						delete ArrayOfNetAddr[ClientNumber];
						ArrayOfNetAddr[ClientNumber] = nullptr;
						uint64_t* TEMPUnusedClientpotIPv4 = (uint64_t*)malloc(TotalNumberOfUnusedClientSpotIPv4 + 1);
						if (TEMPUnusedClientpotIPv4 == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfUnusedClientSpotIPv4 + 1) * sizeof(uint64_t*)) + " Byes Of Memory for UnusedClientpotIPv4 In RemoveClientIPv4 In: NetAddrArray!\n");
						}
						else
						{
							TEMPUnusedClientpotIPv4[TotalNumberOfUnusedClientSpotIPv4] = ClientNumber;//When Current ClientNumber Is the Smallest Of all
							for (uint64_t i = 0; i < TotalNumberOfUnusedClientSpotIPv4; ++i)
							{
								if (ClientNumber > UnusedClientpotIPv4[i])//Checks if ClientNumber Is Bigger
								{
									TEMPUnusedClientpotIPv4[i] = ClientNumber;
									for (i = i; i < TotalNumberOfUnusedClientSpotIPv4; ++i)
									{
										TEMPUnusedClientpotIPv4[i + 1] = UnusedClientpotIPv4[i];
									}
									break;
								}
								else
								{
									//Visual Studio 2019 Is saying Buffer Overrun by 16 bytes for TEMPUnusedClientpotIPv4[i], the writable size is only (TotalNumberOfUnusedClientSpotIPv4 + 1)
									//But Evrything is Correct here? Buffer overrun is impossible!
									TEMPUnusedClientpotIPv4[i] = UnusedClientpotIPv4[i];
								}
							}

							if (UnusedClientpotIPv4 != nullptr)
							{
								free(UnusedClientpotIPv4);
							}
							UnusedClientpotIPv4 = TEMPUnusedClientpotIPv4;
							TotalNumberOfUnusedClientSpotIPv4 = TotalNumberOfUnusedClientSpotIPv4 + 1;
							IsSuccessful = true;

							if (TotalNumberOfUnusedClientSpotIPv4 > MaximumUnusedSpots)
							{
								ClientOrderIPv4** TEMPArrayOfNetAddr = nullptr;
								Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPArrayOfNetAddr, (TotalNumberOfClientsIPv4 - TotalNumberOfUnusedClientSpotIPv4), sizeof(ClientOrderIPv4*), IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfClientsIPv4 - TotalNumberOfUnusedClientSpotIPv4) * sizeof(ClientOrderIPv4*)) + " Byes Of Memory for TEMPArrayOfNetAddr In RemoveClientIPv4 In: NetAddrArray!\n");
								}
								else
								{
									TotalNumberOfClientsIPv4 = TotalNumberOfClientsIPv4 - TotalNumberOfUnusedClientSpotIPv4;//(TotalNumberOfUnusedClientSpotIPv4 - 1);//Minus 1 should be added at the end
									uint64_t j = 0;
									for (uint64_t i = 0; j < TotalNumberOfClientsIPv4; ++i)
									{
										if (ClientNumber == i)
										{
											delete ArrayOfNetAddr[j];
											j = j + 1;
										}
										else
										{
											while (true)
											{
												if (ArrayOfNetAddr[j] == nullptr)
												{
													j = j + 1;
												}
												else
												{
													TEMPArrayOfNetAddr[i] = ArrayOfNetAddr[j];
													break;
												}
											}
										}
										j = j + 1;
									}

									free(ArrayOfNetAddr);
									ArrayOfNetAddr = TEMPArrayOfNetAddr;//PENDING send Updated Client Number to Each Client
									TotalNumberOfClientsIPv4 = TotalNumberOfClientsIPv4 - 1;

									if (UnusedClientpotIPv4 != nullptr)
									{
										free(UnusedClientpotIPv4);
										UnusedClientpotIPv4 = nullptr;
										TotalNumberOfUnusedClientSpotIPv4 = 0;
									}
								}
							}
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv4 Clients Present! in RemoveClientIPv4 In: NetAddrArray!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error RemoveClientIPv4() Failed in NetAddrArray!");
			}
		}

		void RemoveClientIPv6(uint64_t ClientNumber, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveClientIPv6 Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv6)
				{
					if (ListOfClientsIPv6[ClientNumber] == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ClientNumber " + std::to_string(ClientNumber) + " Is Already Removeded from ListOfClientsIPv6 In RemoveClientIPv6 In: NetAddrArray!\n");
						IsSuccessful = false;
					}
					else
					{
						if (ListOfClientsIPv6[ClientNumber]->ClientSocket != NULL)
						{
							closesocket(ListOfClientsIPv6[ClientNumber]->ClientSocket);
						}
						delete ListOfClientsIPv6[ClientNumber];
						uint64_t* TEMPUnusedClientpotIPv6 = (uint64_t*)malloc(TotalNumberOfUnusedClientSpotIPv6 + 1);
						if (TEMPUnusedClientpotIPv6 == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfUnusedClientSpotIPv6 + 1) * sizeof(uint64_t*)) + " Byes Of Memory for UnusedClientpotIPv6 In RemoveClientIPv6 In: NetAddrArray!\n");
						}
						else
						{
							TEMPUnusedClientpotIPv6[TotalNumberOfUnusedClientSpotIPv6] = ClientNumber;//When Current ClientNumber Is the Smallest Of all
							for (uint64_t i = 0; i < TotalNumberOfUnusedClientSpotIPv6; ++i)
							{
								if (ClientNumber > UnusedClientpotIPv6[i])//Checks if ClientNumber Is Bigger
								{
									TEMPUnusedClientpotIPv6[i] = ClientNumber;
									for (i = i; i < TotalNumberOfUnusedClientSpotIPv6; ++i)
									{
										TEMPUnusedClientpotIPv6[i + 1] = UnusedClientpotIPv6[i];
									}
									break;
								}
								else
								{
									//Visual Studio 2019 Is saying Buffer Overrun by 16 bytes for TEMPUnusedClientpotIPv6[i], the writable size is only (TotalNumberOfUnusedClientSpotIPv6 + 1)
									//But Evrything is Correct here? Buffer overrun is impossible!
									TEMPUnusedClientpotIPv6[i] = UnusedClientpotIPv6[i];
								}
							}

							if (UnusedClientpotIPv6 != nullptr)
							{
								free(UnusedClientpotIPv6);
							}
							UnusedClientpotIPv6 = TEMPUnusedClientpotIPv6;
							TotalNumberOfUnusedClientSpotIPv6 = TotalNumberOfUnusedClientSpotIPv6 + 1;
							IsSuccessful = true;

							if (TotalNumberOfUnusedClientSpotIPv6 > MaximumUnusedSpots)
							{
								ClientOrderIPv6** TEMPListOfClientsIPv6 = nullptr;
								Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPListOfClientsIPv6, (TotalNumberOfClientsIPv6 - TotalNumberOfUnusedClientSpotIPv6), sizeof(ClientOrderIPv6*), IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfClientsIPv6 - TotalNumberOfUnusedClientSpotIPv6) * sizeof(ClientOrderIPv6*)) + " Byes Of Memory for TEMPListOfClientsIPv6 In RemoveClientIPv6 In: NetAddrArray!\n");
								}
								else
								{
									TotalNumberOfClientsIPv6 = TotalNumberOfClientsIPv6 - TotalNumberOfUnusedClientSpotIPv6;//(TotalNumberOfUnusedClientSpotIPv6 - 1);//Minus 1 should be added at the end
									uint64_t j = 0;
									for (uint64_t i = 0; j < TotalNumberOfClientsIPv6; ++i)
									{
										if (ClientNumber == i)
										{
											delete ListOfClientsIPv6[j];
											j = j + 1;
										}
										else
										{
											while (true)
											{
												if (ListOfClientsIPv6[j] == nullptr)
												{
													j = j + 1;
												}
												else
												{
													TEMPListOfClientsIPv6[i] = ListOfClientsIPv6[j];
													break;
												}
											}
										}
										j = j + 1;
									}

									free(ListOfClientsIPv6);
									ListOfClientsIPv6 = TEMPListOfClientsIPv6;//PENDING send Updated Client Number to Each Client
									TotalNumberOfClientsIPv6 = TotalNumberOfClientsIPv6 - 1;

									if (UnusedClientpotIPv6 != nullptr)
									{
										free(UnusedClientpotIPv6);
										UnusedClientpotIPv6 = nullptr;
										TotalNumberOfUnusedClientSpotIPv6 = 0;
									}
								}
							}
						}
					}
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv6 Clients Present! in RemoveClientIPv6 In: NetAddrArray!\n");
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error RemoveClientIPv6() Failed in NetAddrArray!");
			}
		}

		void GetClientIPv4(uint64_t ClientNumber, ClientOrderIPv4** ReturnClientIPv4, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetClientIPv4 Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv4)
				{
					*ReturnClientIPv4 = ArrayOfNetAddr[ClientNumber];
					IsSuccessful = true;
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv4 Clients Present! in GetClientIPv4 In: NetAddrArray!\n");
				}
			}
		}

		void GetClientIPv6(uint64_t ClientNumber, ClientOrderIPv6** ReturnClientIPv6, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetClientIPv6 Without Constructing the struct In: NetAddrArray!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv4)
				{
					*ReturnClientIPv6 = ListOfClientsIPv6[ClientNumber];
					IsSuccessful = true;
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv6 Clients Present! in GetClientIPv6 In: NetAddrArray!\n");
				}
			}
		}

		uint64_t GetTotalNumberOfIPv4Clients()
		{
			return TotalNumberOfClientsIPv4;
		}

		uint64_t GetTotalNumberOfIPv6Clients()
		{
			return TotalNumberOfClientsIPv6;
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
							delete ArrayOfNetAddr[i];
							ArrayOfNetAddr[i] = nullptr;
						}
					}
					free(ArrayOfNetAddr);
				}
				if (FreeNetAddarSpotsInArray != nullptr)
				{
					free(FreeNetAddarSpotsInArray);
					FreeNetAddarSpotsInArray = nullptr;
				}

				TotalNumberOfFreeNetAddarSpotsInArray = 0;
				TotalNumberOfNetAddr = 0;

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

	//NOTE: IPv4 List And IPv6 List both has its own client Numbers, Adding Client to IPv4 will not Increase the List of IPv6 and vice-versa
	struct NetworkWrapper
	{
	private:
		bool IsConstructionSuccessful = false;

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

		std::atomic_bool IsProcessingData = false;
		std::atomic_bool IsChangingClientOrder = false;

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
		ClientOrderList* ClientsList = nullptr;//					//NOTE: This is used by Server for List of Clients Connected

		//NOTE: This Is Only  For Client
		ArrayOfNetworkDataAndSize* ClientSentPackets = nullptr;
		ArrayOfNetworkDataAndSize* ClientReceivedPackets = nullptr;
		uint16_t ClientSentCount = 0;//								//NOTE: Counter Resets to 0 When Max SentPacketsArchiveSize		Is Reached, Previous Data at 0 and so on will be Overwritten
		uint16_t ClientReceivedCount = 0;//							//NOTE: Counter Resets to 0 When Max ReceivedPacketsArchiveSize	Is Reached, Previous Data at 0 and so on will be Overwritten

		//PENDING add Atomic Bool for this
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
					NetworkDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
					ClientSentPackets->GetData(ClientSentCount, &PtrToDataAndDataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::GetData Failed in ClientAddSentPackage In: NetworkWrapper!");
					}
					else
					{
						PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error NetworkDataAndSizeStruct::CopyAndStoreData Failed in ClientAddSentPackage In: NetworkWrapper!");
						}
						else
						{
							ClientSentCount = ClientSentCount + 1;
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ClientAddSentPackage() In: NetworkWrapper!");
			}
		}

		//PENDING add Atomic Bool for this
		//NOTE:Command 2Bytes + Data Varied Bytes
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
					NetworkDataAndSizeStruct* PtrToDataAndDataSize = nullptr;
					ClientReceivedPackets->GetData(ClientSentCount, &PtrToDataAndDataSize, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ArrayOfNetworkDataAndSize::GetData Failed in ClientAddReceivedPackage In: NetworkWrapper!");
					}
					else
					{
						PtrToDataAndDataSize->CopyAndStoreData(Data, DataSize, IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error NetworkDataAndSizeStruct::CopyAndStoreData Failed in ClientAddReceivedPackage In: NetworkWrapper!");
						}
						else
						{
							ClientReceivedCount = ClientReceivedCount + 1;
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error ClientAddSentPackage() In: NetworkWrapper!");
			}
		}

		//NOTE: This checks if the sender is in List, and has given correct ClientUniqueID[8 to 15] ("Password" Randomly Generated upon successful connection to The server)
		//NOTE: Received_SizeOfData is the Size of the Data(Designated by the Server/Client) Carried by the Received packet
		void CheckReceivedDataInfoServer(char* ReceivedData, bool TrueForIPv6FalseForIPv4, uint16_t& Received_SizeOfData, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling CheckReceivedDataInfoServer Without Constructing the struct In: NetworkWrapper!\n");
				Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed In: NetworkWrapper!");
			}
			else
			{
				uint64_t Received_ClientNumber = ntohll(((uint64_t*)ReceivedData)[0]);			// [0] to [7] Char
				uint64_t Received_ClientUniqueID = ntohll(((uint64_t*)ReceivedData)[1]);		// [8] to [15] Char
				Received_SizeOfData = ntohll(((uint16_t*)ReceivedData)[8]);						//[16] to [17] Char
				//uint16_t Received_NetworkWrapperCommand = ntohll(((uint16_t*)ReceivedData)[9]);	//[18] to [19] Char //NOT NEEDED HERE!

				if (TrueForIPv6FalseForIPv4)
				{
					ClientOrderIPv6* ClientOrderIPv6ptr = nullptr;
					ClientsList->GetClientIPv6(Received_ClientNumber, &ClientOrderIPv6ptr, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv6() Failed in CheckReceivedDataInfoServer In: NetworkWrapper!");
					}
					else
					{
						if (Received_ClientUniqueID != ClientOrderIPv6ptr->ClientUniqueID)
						{
							Essenbp::WriteLogToFile("\n Error Received_ClientUniqueID Does not Match With ClientUniqueID in CheckReceivedDataInfoServer In: NetworkWrapper!");
						}
						else
						{
							if (Received_SizeOfData > MaxDataSizePerPacket)
							{
								Essenbp::WriteLogToFile("\n Error Received_SizeOfData Exceeds SizeOfData in CheckReceivedDataInfoServer In: NetworkWrapper!");
							}
							else
							{
								IsSuccessful = true;
							}
						}
					}
				}
				else
				{
					ClientOrderIPv4* ClientOrderIPv4ptr = nullptr;
					ClientsList->GetClientIPv4(Received_ClientNumber, &ClientOrderIPv4ptr, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv4() Failed in CheckReceivedDataInfoServer In: NetworkWrapper!");
					}
					else
					{
						if (Received_ClientUniqueID != ClientOrderIPv4ptr->ClientUniqueID)
						{
							Essenbp::WriteLogToFile("\n Error Received_ClientUniqueID Does not Match With ClientUniqueID in CheckReceivedDataInfoServer In: NetworkWrapper!");
						}
						else
						{
							if (Received_SizeOfData > MaxDataSizePerPacket)
							{
								Essenbp::WriteLogToFile("\n Error Received_SizeOfData Exceeds SizeOfData in CheckReceivedDataInfoServer In: NetworkWrapper!");
							}
							else
							{
								IsSuccessful = true;
							}
						}
					}
				}
			}

			//if (!IsSuccessful)//NO Need here because This is Received Data From The Internet...  //CHECK PENDING
			//{
			//	Essenbp::WriteLogToFile("\n Error CheckReceivedDataInfoServer Failed In: NetworkWrapper!");
			//}
		}

		void CheckAndGetReceivedDataInfoServer(char* ReceivedData, bool TrueForIPv6FalseForIPv4, uint16_t& Received_SizeOfData, uint64_t& Received_ClientNumber, uint64_t& Received_ClientUniqueID, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling CheckAndGetReceivedDataInfoServer Without Constructing the struct In: NetworkWrapper!\n");
				Essenbp::WriteLogToFile("\n Error CheckAndGetReceivedDataInfoServer Failed In: NetworkWrapper!");
			}
			else
			{
				Received_ClientNumber = ntohll(((uint64_t*)ReceivedData)[0]);					// [0] to [7] Char
				Received_ClientUniqueID = ntohll(((uint64_t*)ReceivedData)[1]);					// [8] to [15] Char
				Received_SizeOfData = ntohll(((uint16_t*)ReceivedData)[8]);						//[16] to [17] Char
				//uint16_t Received_NetworkWrapperCommand = ntohll(((uint16_t*)ReceivedData)[9]);	//[18] to [19] Char //NOT NEEDED HERE!

				if (TrueForIPv6FalseForIPv4)
				{
					ClientOrderIPv6* ClientOrderIPv6ptr = nullptr;
					ClientsList->GetClientIPv6(Received_ClientNumber, &ClientOrderIPv6ptr, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv6() Failed in CheckAndGetReceivedDataInfoServer In: NetworkWrapper!");
					}
					else
					{
						if (Received_ClientUniqueID != ClientOrderIPv6ptr->ClientUniqueID)
						{
							Essenbp::WriteLogToFile("\n Error Received_ClientUniqueID Does not Match With ClientUniqueID in CheckAndGetReceivedDataInfoServer In: NetworkWrapper!");
						}
						else
						{
							if (Received_SizeOfData > MaxDataSizePerPacket)
							{
								Essenbp::WriteLogToFile("\n Error Received_SizeOfData Exceeds SizeOfData in CheckAndGetReceivedDataInfoServer In: NetworkWrapper!");
							}
							else
							{
								IsSuccessful = true;
							}
						}
					}
				}
				else
				{
					ClientOrderIPv4* ClientOrderIPv4ptr = nullptr;
					ClientsList->GetClientIPv4(Received_ClientNumber, &ClientOrderIPv4ptr, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv4() Failed in CheckAndGetReceivedDataInfoServer In: NetworkWrapper!");
					}
					else
					{
						if (Received_ClientUniqueID != ClientOrderIPv4ptr->ClientUniqueID)
						{
							Essenbp::WriteLogToFile("\n Error Received_ClientUniqueID Does not Match With ClientUniqueID in CheckAndGetReceivedDataInfoServer In: NetworkWrapper!");
						}
						else
						{
							if (Received_SizeOfData > MaxDataSizePerPacket)
							{
								Essenbp::WriteLogToFile("\n Error Received_SizeOfData Exceeds SizeOfData in CheckAndGetReceivedDataInfoServer In: NetworkWrapper!");
							}
							else
							{
								IsSuccessful = true;
							}
						}
					}
				}
			}

			//if (!IsSuccessful)//NO Need here because This is Received Data From The Internet...  //CHECK PENDING
			//{
			//	Essenbp::WriteLogToFile("\n Error CheckAndGetReceivedDataInfoServer Failed In: NetworkWrapper!");
			//}
		}

		void SendDataUDP(const sockaddr_in* DestinationAddress, NetworkDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
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
					sendto(UDPServerSocketIPv4, DataAndSize.GetData(), DataAndSize.GetDataSize(), 0, (sockaddr*)DestinationAddress, sizeof(*DestinationAddress));
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendDataUDP Failed In: NetworkWrapper!");
			}
		}		

		void SendDataUDP(const sockaddr_in6* DestinationAddress, NetworkDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
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
					sendto(UDPServerSocketIPv6, DataAndSize.GetData(), DataAndSize.GetDataSize(), 0, (sockaddr*)DestinationAddress, sizeof(*DestinationAddress));
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendDataUDP Failed In: NetworkWrapper!");
			}
		}

		//NOTE:Server = TCP ONly
		//NOTE:Client = UDP(Connected) And TCP
		void SendDataTCPUDP(const SOCKET* DestinationSOCKET, NetworkDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendDataTCPUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				send(*DestinationSOCKET, DataAndSize.GetData(), DataAndSize.GetDataSize(), 0);
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

	public:
		//For Server to Client
		void SendData(uint64_t ClientNumber, NetworkDataAndSizeStruct& DataAndSize, bool TrueForIPv6FalseForIPv4, bool& IsSuccessful)
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
					if (TrueForIPv6FalseForIPv4)
					{
						ClientOrderIPv6* ClientOrderIPv6ptr = nullptr;
						ClientsList->GetClientIPv6(ClientNumber, &ClientOrderIPv6ptr, IsSuccessful);
						if (IsSuccessful)
						{
							if (ClientOrderIPv6ptr->ClientSocket != NULL)
							{
								SendDataTCPUDP(&(ClientOrderIPv6ptr->ClientSocket), DataAndSize, IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
								}
							}
							else
							{
								SendDataUDP(&(ClientOrderIPv6ptr->ClientAddress), DataAndSize, IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SendDataUDP() Failed in SendData In: NetworkWrapper!");
								}
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv6() Failed in SendData In: NetworkWrapper!");
						}
					}
					else
					{
						ClientOrderIPv4* ClientOrderIPv4ptr = nullptr;
						ClientsList->GetClientIPv4(ClientNumber, &ClientOrderIPv4ptr, IsSuccessful);
						if (IsSuccessful)
						{
							if (ClientOrderIPv4ptr->ClientSocket != NULL)
							{
								SendDataTCPUDP(&(ClientOrderIPv4ptr->ClientSocket), DataAndSize, IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
								}
							}
							else
							{
								SendDataUDP(&(ClientOrderIPv4ptr->ClientAddress), DataAndSize, IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error SendDataUDP() Failed in SendData In: NetworkWrapper!");
								}
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv4() Failed in SendData In: NetworkWrapper!");
						}
					}
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendData Failed In: NetworkWrapper!");
			}
		}

		//For Client To Server
		void SendData(NetworkDataAndSizeStruct& DataAndSize, bool TrueForIPv6FalseForIPv4, bool& IsSuccessful)
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
						if (IsClientTrueForTCPFalseForUDP)
						{
							SendDataTCPUDP(&TCPServerSocketIPv6, DataAndSize, IsSuccessful);
						}
						else
						{
							SendDataTCPUDP(&UDPServerSocketIPv6, DataAndSize, IsSuccessful);
						}
						
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error SendDataTCPUDP() Failed in SendData In: NetworkWrapper!");
						}
					}
					else
					{
						if (IsClientTrueForTCPFalseForUDP)
						{
							SendDataTCPUDP(&TCPServerSocketIPv4, DataAndSize, IsSuccessful);
						}
						else
						{
							SendDataTCPUDP(&UDPServerSocketIPv4, DataAndSize, IsSuccessful);
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
					else
					{
						ClientAddSentPackage(DataAndSize.GetData(), DataAndSize.GetDataSize(), IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("Error ClientAddSentPackage() Failed in ReceiveDataTCPUDPForClient In: NetworkWrapper!");
						}
					}					
				}
			}

			if (!IsSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error SendData Failed In: NetworkWrapper!");
			}
		}	
	
	private:
										/*NetworkWrapper Command Number and It's Functions List*/
		/****************************************************************************************************************************///PENDING
		//		NetworkWrapper Specific Info/Commands = (8 Byte ClientNumber, 8 Byte Client Unique ID, 2 Byte SizeOfData, 2 Byte Command)
		//					 ClientUniqueID[8 to 15] ("Password" Randomly Generated upon successful connection to The server)
		//						   Network Wrapper Command Byte Meaning (ReceivedData[18] & [19] Is the Command Byte)
		//											   Functions Defination Declared At the Last				
		// 
		//
		//														/*Server*/
		/*
		* 0.) Connect to Server / Disconnect from Server Confirmation
		* 1.) Normal Passage Of Data(+ 2 Bytes Command)
		*/
		//
		//
		//														/*CLIENT*/
		/*
		* 0.) Connect to Server / Disconnect from Server Request/Confirmation
		* 1.) Normal Passage Of Data(+ 2 Bytes Command)
		*/
		/****************************************************************************************************************************/
							
										/*NetworkWrapper Command Number Specific Functions Defination List*/
		/****************************************************************************************************************************///PENDING
		//													/*Server*/
		//Command = 0, Verify Connection of Client to Server / Disconnect Client from Server
		void ServerSideConnectionDisconnectionConfirmationTCP(SOCKET ClientSocket, bool& IsSuccessful, bool IsInitialJoin = false, uint8_t MaxVerificationTries = 2)
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
					NetworkDataAndSizeStruct DataAndSize;
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
		void ServerSideConnectionDisconnectionConfirmationUDP(bool SendTempPassword_Or_VerifyReceivedTempPassword, const sockaddr_in* DestinationAddress, NetworkDataAndSizeStruct& SentDataOrReceivedData, uint32_t& TempPassword, bool& IsSuccessful)
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
		void ServerSideConnectionDisconnectionConfirmationUDP(bool SendTempPassword_Or_VerifyReceivedTempPassword, const sockaddr_in6* DestinationAddress, NetworkDataAndSizeStruct& SentDataOrReceivedData, uint32_t& TempPassword, bool& IsSuccessful)
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
					NetworkDataAndSizeStruct SentDataOrReceivedData;
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
					ClientSentPackets = new ArrayOfNetworkDataAndSize;
					if (ClientSentPackets != nullptr)
					{
						ClientReceivedPackets = new ArrayOfNetworkDataAndSize;
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