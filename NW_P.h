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

#define _SOCKCleanup
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
							for (size_t i = 0; i < SizeOfData; ++i)// Memccpy bad
							{
								Data[i] = ((char*)ArgData)[i];// I could simply convert void* to char*... but i left it as void* for the purpose of 'readability'
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

		//NOTE:Use this if Direct access to data is required
		//NOTE: by using this
		void FreeAndResizeData(size_t ArgSizeOfData, bool& Issuccessful)
		{
			Issuccessful = false;
			FreeData();

			SizeOfData = ArgSizeOfData;
			if (SizeOfData == 0)
			{
				Essenbp::WriteLogToFile("\n Error Size Of SizeOfData is Equal to Zero in FreeAndResizeDataAndReturnPointerToDataPointer in NetworkDataAndSizeStruct In: Essenbp!\n");
				return;
			}
			Data = (char*)malloc(SizeOfData);
			if (Data == nullptr)
			{
				Essenbp::WriteLogToFile("\n Error Allocating : " + std::to_string(SizeOfData) + " Byes Of Memory for Data in FreeAndResizeDataAndReturnPointerToDataPointer in NetworkDataAndSizeStruct In: Essenbp!\n");
				return;
			}
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
				Essenbp::WriteLogToFile("\n Error Argument ReturnData is nullptr in GetDataAndSizeAndSetDataToNull in NetworkDataAndSizeStruct In: Essenbp!\n");
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

	public:
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

	struct ClientOrderIPv4
	{
	public:
		const SOCKET ClientSocket;
		const uint64_t ClientNumber;
		const sockaddr_in ClientAddress;

		bool IsConstructionSuccessful = false;

		//For UDP set ArgClientSocket == NULL
		ClientOrderIPv4(SOCKET ArgClientSocket, uint64_t ArgClientNumber, sockaddr_in ArgClientAddress) : ClientSocket(ArgClientSocket), ClientNumber(ArgClientNumber), ClientAddress(ArgClientAddress)
		{
			Essenbp::WriteLogToFile("\n Constructing ClientOrderIPv4!");
			IsConstructionSuccessful = true;
		}

		~ClientOrderIPv4()
		{
			Essenbp::WriteLogToFile("\n Destructing ClientOrderIPv4!");
			if (IsConstructionSuccessful)
			{
				if (ClientSocket != NULL)
				{
					closesocket(ClientSocket);
				}
				IsConstructionSuccessful = false;
			}
		}
	};

	struct ClientOrderIPv6
	{
	public:
		const SOCKET ClientSocket;
		const uint64_t ClientNumber;
		const sockaddr_in6 ClientAddress;

		bool IsConstructionSuccessful = false;

		//For UDP set ArgClientSocket == NULL
		ClientOrderIPv6(SOCKET ArgClientSocket, uint64_t ArgClientNumber, sockaddr_in6 ArgClientAddress) : ClientSocket(ArgClientSocket), ClientNumber(ArgClientNumber), ClientAddress(ArgClientAddress)
		{
			Essenbp::WriteLogToFile("\n Constructing ClientOrderIPv6!");
			IsConstructionSuccessful = true;
		}

		~ClientOrderIPv6()
		{
			Essenbp::WriteLogToFile("\n Destructing ClientOrderIPv6!");
			if (IsConstructionSuccessful)
			{
				if (ClientSocket != NULL)
				{
					closesocket(ClientSocket);
				}
				IsConstructionSuccessful = false;
			}
		}
	};

	//Only For Server
	struct ClientOrderList
	{
	private:
		ClientOrderIPv4** ListOfClientsIPv4 = nullptr;
		ClientOrderIPv6** ListOfClientsIPv6 = nullptr;
		uint64_t TotalNumberOfClientsIPv4 = 0;
		uint64_t TotalNumberOfClientsIPv6 = 0;

		const uint64_t MaximumUnusedSpots = 0;//Default is 2048, If there are More than or Equal to 2048 Spots free spots the entire ClintOrderList will be Reordered...
		uint64_t* UnusedClientpotIPv4 = nullptr;
		uint64_t* UnusedClientpotIPv6 = nullptr;
		uint64_t TotalNumberOfUnusedClientSpotIPv4 = 0;
		uint64_t TotalNumberOfUnusedClientSpotIPv6 = 0;

		bool IsConstructionSuccessful = false;

	public:
		ClientOrderList(uint64_t ArgMaximumUnusedSpots) : MaximumUnusedSpots(ArgMaximumUnusedSpots)
		{
			Essenbp::WriteLogToFile("\n Constructing ClientOrderList!");

			ListOfClientsIPv4 = nullptr;
			ListOfClientsIPv6 = nullptr;
			TotalNumberOfClientsIPv4 = 0;
			TotalNumberOfClientsIPv6 = 0;

			UnusedClientpotIPv4 = nullptr;
			UnusedClientpotIPv6 = nullptr;
			TotalNumberOfUnusedClientSpotIPv4 = 0;
			TotalNumberOfUnusedClientSpotIPv6 = 0;

			IsConstructionSuccessful = true;
		}

		void AddClientIPv4(SOCKET ClientSocket, sockaddr_in ClientAddress, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddClientIPv4 Without Constructing the struct In: ClientOrderList!\n");
			}
			else
			{
				if (TotalNumberOfClientsIPv4 > 0)
				{
					if (TotalNumberOfClientsIPv4 == 1)
					{
						ListOfClientsIPv4[0] = new ClientOrderIPv4(ClientSocket, TotalNumberOfClientsIPv4, ClientAddress);// NO need for IsSuccesful Constrction check as it is not needed in this simple struct
						if (ListOfClientsIPv4[0] == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderIPv4)) + " Byes Of Memory for ListOfClientsIPv4[0] in AddClientIPv4 In: ClientOrderList!\n");
						}
						else
						{
							free(UnusedClientpotIPv4);
							UnusedClientpotIPv4 = nullptr;
							IsSuccessful = true;
						}
					}
					else
					{
						uint64_t* TEMPUnusedClientpotIPv4 = (uint64_t*)malloc(TotalNumberOfUnusedClientSpotIPv4 - 1);
						if (TEMPUnusedClientpotIPv4 == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfUnusedClientSpotIPv4 - 1) * sizeof(uint64_t*)) + " Byes Of Memory for UnusedClientpotIPv4 In AddClientIPv4 In: ClientOrderList!\n");
						}
						else
						{
							ListOfClientsIPv4[UnusedClientpotIPv4[(TotalNumberOfUnusedClientSpotIPv4 - 1)]] = new ClientOrderIPv4(ClientSocket, TotalNumberOfClientsIPv4, ClientAddress);// NO need for IsSuccesful Constrction check as it is not needed in this simple struct
							if (ListOfClientsIPv4[UnusedClientpotIPv4[(TotalNumberOfUnusedClientSpotIPv4 - 1)]] == nullptr)
							{
								Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderIPv4)) + " Byes Of Memory for ListOfClientsIPv4[" + std::to_string(UnusedClientpotIPv4[(TotalNumberOfUnusedClientSpotIPv4 - 1)]) + "] in AddClientIPv4 In: ClientOrderList!\n");
							}
							else
							{
								TotalNumberOfUnusedClientSpotIPv4 = TotalNumberOfUnusedClientSpotIPv4 - 1;
								for (uint64_t i = 0; i < TotalNumberOfUnusedClientSpotIPv4; ++i)
								{
									//Visual Studio 2019 Is saying Buffer Overrun by 16 bytes for TEMPUnusedClientpotIPv4[i], the writable size is only (TotalNumberOfUnusedClientSpotIPv4 - 1)
									//But Evrything is Correct here? Buffer overrun is impossible!
									TEMPUnusedClientpotIPv4[i] = UnusedClientpotIPv4[i];
								}
								free(UnusedClientpotIPv4);
								UnusedClientpotIPv4 = TEMPUnusedClientpotIPv4;
								IsSuccessful = true;
							}
						}
					}
				}

				ClientOrderIPv4** TEMPListOfClientsIPv4 = nullptr;
				Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPListOfClientsIPv4, (TotalNumberOfClientsIPv4 + 1), sizeof(ClientOrderIPv4*), IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfClientsIPv4 + 1) * sizeof(ClientOrderIPv4*)) + " Byes Of Memory for TEMPListOfClientsIPv4 In AddClientIPv4 In: ClientOrderList!\n");
				}
				else
				{
					TEMPListOfClientsIPv4[TotalNumberOfClientsIPv4] = new ClientOrderIPv4(ClientSocket, TotalNumberOfClientsIPv4, ClientAddress);// NO need for IsSuccesful Constrction check as it is not needed in this simple struct
					if (TEMPListOfClientsIPv4[TotalNumberOfClientsIPv4] == nullptr)
					{
						IsSuccessful = false;
					}
					else
					{
						for (uint64_t i = 0; i < TotalNumberOfClientsIPv4; ++i)
						{
							TEMPListOfClientsIPv4[i] = ListOfClientsIPv4[i];
						}

						free(ListOfClientsIPv4);
						ListOfClientsIPv4 = TEMPListOfClientsIPv4;
						TotalNumberOfClientsIPv4 = TotalNumberOfClientsIPv4 + 1;
					}

					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderIPv4)) + " Byes Of Memory for TEMPListOfClientsIPv4[" + std::to_string(TotalNumberOfClientsIPv4) + "] in AddClientIPv4 In: ClientOrderList!\n");
					}
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error AddClientIPv4() Failed in ClientOrderList!");
			}
		}

		void AddClientIPv6(SOCKET ClientSocket, sockaddr_in6 ClientAddress, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling AddClientIPv6 Without Constructing the struct In: ClientOrderList!\n");
			}
			else
			{
				if (TotalNumberOfClientsIPv6 > 0)
				{
					if (TotalNumberOfClientsIPv6 == 1)
					{
						ListOfClientsIPv6[0] = new ClientOrderIPv6(ClientSocket, TotalNumberOfClientsIPv6, ClientAddress);// NO need for IsSuccesful Constrction check as it is not needed in this simple struct
						if (ListOfClientsIPv6[0] == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderIPv6)) + " Byes Of Memory for ListOfClientsIPv6[0] in AddClientIPv6 In: ClientOrderList!\n");
						}
						else
						{
							free(UnusedClientpotIPv6);
							UnusedClientpotIPv6 = nullptr;
							IsSuccessful = true;
						}
					}
					else
					{
						uint64_t* TEMPUnusedClientpotIPv6 = (uint64_t*)malloc(TotalNumberOfUnusedClientSpotIPv6 - 1);
						if (TEMPUnusedClientpotIPv6 == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfUnusedClientSpotIPv6 - 1) * sizeof(uint64_t*)) + " Byes Of Memory for UnusedClientpotIPv6 In AddClientIPv6 In: ClientOrderList!\n");
						}
						else
						{
							ListOfClientsIPv6[UnusedClientpotIPv6[(TotalNumberOfUnusedClientSpotIPv6 - 1)]] = new ClientOrderIPv6(ClientSocket, TotalNumberOfClientsIPv6, ClientAddress);// NO need for IsSuccesful Constrction check as it is not needed in this simple struct
							if (ListOfClientsIPv6[UnusedClientpotIPv6[(TotalNumberOfUnusedClientSpotIPv6 - 1)]] == nullptr)
							{
								Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderIPv6)) + " Byes Of Memory for ListOfClientsIPv6[" + std::to_string(UnusedClientpotIPv6[(TotalNumberOfUnusedClientSpotIPv6 - 1)]) + "] in AddClientIPv6 In: ClientOrderList!\n");
							}
							else
							{
								TotalNumberOfUnusedClientSpotIPv6 = TotalNumberOfUnusedClientSpotIPv6 - 1;
								for (uint64_t i = 0; i < TotalNumberOfUnusedClientSpotIPv6; ++i)
								{
									//Visual Studio 2019 Is saying Buffer Overrun by 16 bytes for TEMPUnusedClientpotIPv6[i], the writable size is only (TotalNumberOfUnusedClientSpotIPv6 - 1)
									//But Evrything is Correct here? Buffer overrun is impossible!
									TEMPUnusedClientpotIPv6[i] = UnusedClientpotIPv6[i];
								}
								free(UnusedClientpotIPv6);
								UnusedClientpotIPv6 = TEMPUnusedClientpotIPv6;
								IsSuccessful = true;
							}
						}
					}
				}

				ClientOrderIPv6** TEMPListOfClientsIPv6 = nullptr;
				Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPListOfClientsIPv6, (TotalNumberOfClientsIPv6 + 1), sizeof(ClientOrderIPv6*), IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfClientsIPv6 + 1) * sizeof(ClientOrderIPv6*)) + " Byes Of Memory for TEMPListOfClientsIPv6 In AddClientIPv6 In: ClientOrderList!\n");
				}
				else
				{
					TEMPListOfClientsIPv6[TotalNumberOfClientsIPv6] = new ClientOrderIPv6(ClientSocket, TotalNumberOfClientsIPv6, ClientAddress);// NO need for IsSuccesful Constrction check as it is not needed in this simple struct
					if (TEMPListOfClientsIPv6[TotalNumberOfClientsIPv6] == nullptr)
					{
						IsSuccessful = false;
					}
					else
					{
						for (uint64_t i = 0; i < TotalNumberOfClientsIPv6; ++i)
						{
							TEMPListOfClientsIPv6[i] = ListOfClientsIPv6[i];
						}

						free(ListOfClientsIPv6);
						ListOfClientsIPv6 = TEMPListOfClientsIPv6;
						TotalNumberOfClientsIPv6 = TotalNumberOfClientsIPv6 + 1;
					}

					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderIPv6)) + " Byes Of Memory for TEMPListOfClientsIPv6[" + std::to_string(TotalNumberOfClientsIPv6) + "] in AddClientIPv6 In: ClientOrderList!\n");
					}
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error AddClientIPv6() Failed in ClientOrderList!");
			}
		}

		void RemoveClientIPv4(uint64_t ClientNumber, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveClientIPv4 Without Constructing the struct In: ClientOrderList!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv4)
				{
					if (ListOfClientsIPv4[ClientNumber] == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ClientNumber " + std::to_string(ClientNumber) + " Is Already Removeded from ListOfClientsIPv4 In RemoveClientIPv4 In: ClientOrderList!\n");
						IsSuccessful = false;
					}
					else
					{
						if (ListOfClientsIPv4[ClientNumber]->ClientSocket != NULL)
						{
							closesocket(ListOfClientsIPv4[ClientNumber]->ClientSocket);
						}
						delete ListOfClientsIPv4[ClientNumber];
						ListOfClientsIPv4[ClientNumber] = nullptr;
						uint64_t* TEMPUnusedClientpotIPv4 = (uint64_t*)malloc(TotalNumberOfUnusedClientSpotIPv4 + 1);
						if (TEMPUnusedClientpotIPv4 == nullptr)
						{
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfUnusedClientSpotIPv4 + 1) * sizeof(uint64_t*)) + " Byes Of Memory for UnusedClientpotIPv4 In RemoveClientIPv4 In: ClientOrderList!\n");
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
								ClientOrderIPv4** TEMPListOfClientsIPv4 = nullptr;
								Essenbp::Malloc_PointerToArrayOfPointers((void***)&TEMPListOfClientsIPv4, (TotalNumberOfClientsIPv4 - TotalNumberOfUnusedClientSpotIPv4), sizeof(ClientOrderIPv4*), IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfClientsIPv4 - TotalNumberOfUnusedClientSpotIPv4) * sizeof(ClientOrderIPv4*)) + " Byes Of Memory for TEMPListOfClientsIPv4 In RemoveClientIPv4 In: ClientOrderList!\n");
								}
								else
								{
									TotalNumberOfClientsIPv4 = TotalNumberOfClientsIPv4 - TotalNumberOfUnusedClientSpotIPv4;//(TotalNumberOfUnusedClientSpotIPv4 - 1);//Minus 1 should be added at the end
									uint64_t j = 0;
									for (uint64_t i = 0; j < TotalNumberOfClientsIPv4; ++i)
									{
										if (ClientNumber == i)
										{
											delete ListOfClientsIPv4[j];
											j = j + 1;
										}
										else
										{
											while (true)
											{
												if (ListOfClientsIPv4[j] == nullptr)
												{
													j = j + 1;	
												}
												else
												{													
													TEMPListOfClientsIPv4[i] = ListOfClientsIPv4[j];
													break;
												}
											}											
										}
										j = j + 1;
									}

									free(ListOfClientsIPv4);
									ListOfClientsIPv4 = TEMPListOfClientsIPv4;//PENDING send Updated Client Number to Each Client
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
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv4 Clients Present! in RemoveClientIPv4 In: ClientOrderList!\n");
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error RemoveClientIPv4() Failed in ClientOrderList!");
			}
		}

		void RemoveClientIPv6(uint64_t ClientNumber, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling RemoveClientIPv6 Without Constructing the struct In: ClientOrderList!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv6)
				{
					if (ListOfClientsIPv6[ClientNumber] == nullptr)
					{
						Essenbp::WriteLogToFile("\n Error ClientNumber " + std::to_string(ClientNumber) + " Is Already Removeded from ListOfClientsIPv6 In RemoveClientIPv6 In: ClientOrderList!\n");
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
							Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfUnusedClientSpotIPv6 + 1) * sizeof(uint64_t*)) + " Byes Of Memory for UnusedClientpotIPv6 In RemoveClientIPv6 In: ClientOrderList!\n");
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
									Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string((TotalNumberOfClientsIPv6 - TotalNumberOfUnusedClientSpotIPv6) * sizeof(ClientOrderIPv6*)) + " Byes Of Memory for TEMPListOfClientsIPv6 In RemoveClientIPv6 In: ClientOrderList!\n");
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
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv6 Clients Present! in RemoveClientIPv6 In: ClientOrderList!\n");
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error RemoveClientIPv6() Failed in ClientOrderList!");
			}
		}

		void GetClientIPv4(uint64_t ClientNumber, ClientOrderIPv4** ReturnClientIPv4, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetClientIPv4 Without Constructing the struct In: ClientOrderList!\n");
			}
			else
			{
				if (ClientNumber < TotalNumberOfClientsIPv4)
				{
					*ReturnClientIPv4 = ListOfClientsIPv4[ClientNumber];
					IsSuccessful = true;
				}
				else
				{
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv4 Clients Present! in GetClientIPv4 In: ClientOrderList!\n");
				}
			}
		}

		void GetClientIPv6(uint64_t ClientNumber, ClientOrderIPv6** ReturnClientIPv6, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling GetClientIPv6 Without Constructing the struct In: ClientOrderList!\n");
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
					Essenbp::WriteLogToFile("\n Error ClientNumber + 1 Exceeds the Number Of IPv6 Clients Present! in GetClientIPv6 In: ClientOrderList!\n");
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

		~ClientOrderList()
		{
			Essenbp::WriteLogToFile("\n Destructing ClientOrderList!");
			if (IsConstructionSuccessful)
			{
				for (uint64_t i = 0; i < TotalNumberOfClientsIPv4; ++i)
				{
					delete ListOfClientsIPv4[i];
				}
				for (uint64_t i = 0; i < TotalNumberOfClientsIPv6; ++i)
				{
					delete ListOfClientsIPv6[i];
				}
				free(ListOfClientsIPv4);
				free(ListOfClientsIPv6);

				ListOfClientsIPv4 = nullptr;
				ListOfClientsIPv6 = nullptr;
				TotalNumberOfClientsIPv4 = 0;
				TotalNumberOfClientsIPv6 = 0;

				IsConstructionSuccessful = false;
			}
		}
	};

	//NOTE: IPv4 List And IPv6 List both has its own client Numbers, Adding Client to IPv4 will not Increase the List of IPv6 and vice-versa
	struct NetworkWrapper
	{
	private:
		const bool IsServer;
		const bool TrueForTCPFalseForUDP;
		SOCKET SocketIPv4 = NULL;
		SOCKET SocketIPv6 = NULL;
		sockaddr_in ServerHintIPv4 = { 0 };//This Server or Connecting Server hint
		sockaddr_in6 ServerHintIPv6 = { 0 };//This Server or Connecting Server hint

		ClientOrderList* ClientsList;

#ifdef _WIN32
		DWORD InputTimeOut = 0;//TCP SOCKET InputTimeOut			//NOTE: will reuse this for UDP custom Time out
		DWORD OutputTimeOut = 0;//TCP SOCKET OutputTimeOut			//NOTE: will reuse this for UDP custom Time out
#else
		unsigned long InputTimeOut = 0;//TCP SOCKET InputTimeOut	//NOTE: will reuse this for UDP custom Time out
		unsigned long OutputTimeOut = 0;//TCP SOCKET OutputTimeOut	//NOTE: will reuse this for UDP custom Time out 
#endif
		int MaximumBackLogConnectionsTCP = 5;

		void SendDataUDP(const sockaddr_in* DestinationAddress, NetworkDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendDataUDP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				sendto(SocketIPv4, DataAndSize.GetData(), DataAndSize.GetDataSize(), 0, (sockaddr*)DestinationAddress, sizeof(*DestinationAddress));
			}

			if (!IsSuccessful)// For the safe of readability
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
				sendto(SocketIPv6, DataAndSize.GetData(), DataAndSize.GetDataSize(), 0, (sockaddr*)DestinationAddress, sizeof(*DestinationAddress));
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error SendDataUDP Failed In: NetworkWrapper!");
			}
		}

		void SendDataTCP(const SOCKET* DestinationSOCKET, NetworkDataAndSizeStruct& DataAndSize, bool& IsSuccessful)
		{
			IsSuccessful = false;

			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling SendDataTCP Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				send(*DestinationSOCKET, DataAndSize.GetData(), DataAndSize.GetDataSize(), 0);
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error SendDataTCP Failed In: NetworkWrapper!");
			}
		}

	public:
		bool IsConstructionSuccessful = false;

		//NOTE: If IPv4/IPv6 Is Present And If IPv6/IPv4 is also needed then Run this function
		void CreateSocket(std::string IPAddress, unsigned int PortNumber, bool TrueForIPv6FalseForIPv4, bool& IsSuccessful)
		{
			IsSuccessful = true;
			SOCKET* Socketptr = nullptr;
			if (!IsConstructionSuccessful)
			{
				Essenbp::WriteLogToFile("\n Error Calling CreateSocket Without Constructing the struct In: NetworkWrapper!\n");
			}
			else
			{
				if (TrueForIPv6FalseForIPv4)
				{
					if (SocketIPv6 != NULL)
					{
						Essenbp::WriteLogToFile("\n Error IPv6 Socket Already Exists in CreateSocket In: NetworkWrapper!");
						IsSuccessful = false;
					}
				}
				else
				{
					if (SocketIPv4 != NULL)
					{
						Essenbp::WriteLogToFile("\n Error IPv4 Socket Already Exists in CreateSocket In: NetworkWrapper!");
						IsSuccessful = false;
					}
				}

				if (IsSuccessful)
				{
					if (TrueForTCPFalseForUDP)
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

							if (IsServer)
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

							if (IsServer)
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
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error CreateSocket Failed In: NetworkWrapper!");
			}
		}

		//For Server And Client
		NetworkWrapper(std::string IPAddress, unsigned int PortNumber, bool TrueForServerFalseForClient, bool TrueForIPv6FalseForIPv4, bool ArgTrueForTCPFalseForUDP, bool& IsSuccessful, int SocketInputTimeoutInSeconds = 60, int SocketOutputTimeoutInSeconds = 60, uint64_t MaximumUnusedClientSpots = 2048, int ArgMaximumBackLogConnectionsTCP_ONLY_FOR_TCP = 5) : IsServer(TrueForServerFalseForClient), TrueForTCPFalseForUDP(ArgTrueForTCPFalseForUDP)
		{
			Essenbp::WriteLogToFile("\n Constructing NetworkWrapper!");

			if (!EndianCheckDone)
			{
				NW_PCheckIfLittleEndian();
			}

			SocketIPv4 = NULL;
			SocketIPv6 = NULL;
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
			MaximumBackLogConnectionsTCP = ArgMaximumBackLogConnectionsTCP_ONLY_FOR_TCP;//Does not Matter for UDP
			ClientsList = new ClientOrderList(MaximumUnusedClientSpots);
			if (ClientsList == nullptr)
			{
				Essenbp::WriteLogToFile("\n Error Allocating " + std::to_string(sizeof(ClientOrderList)) + " Byes Of Memory for ClientsList in AddClientIPv6 In: NetworkWrapper!\n");
				IsSuccessful = false;
			}

			if (IsSuccessful)
			{
				SOCKET* Socketptr = nullptr;

				InputTimeOut = SocketInputTimeoutInSeconds * 1000;//60 Seconds InputTimeOut
				OutputTimeOut = SocketOutputTimeoutInSeconds * 1000;//60 Seconds OutputTimeOut

				IsConstructionSuccessful = true;//Temp for the CreateSocket Function
				CreateSocket(IPAddress, PortNumber, TrueForIPv6FalseForIPv4, IsSuccessful);
			}		

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error Construction Failed NetworkWrapper!");
				IsConstructionSuccessful = false;
			}
			else
			{
				IsConstructionSuccessful = true;
			}
		}		

		void ReceiveDataThreadIPv4TCP(ClientOrderIPv4 ClientInfo)
		{
			int ClientLength = sizeof(ClientInfo.ClientAddress); // The size of the client information	
			char ReceivedData[VARIABLESIZESETNOW];//128 Is Not Fully Used// 0 is Checknumber(>20 means Ability Casting or interactions) (0-5)6 Char(1Byte x  6) Per Client For Movement And Ability/Action Selection, The Remaining (6-11)6 Char(1Byte x  6) Is For Location			

			setsockopt(ClientInfo.ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&InputTimeOut, sizeof(InputTimeOut));
			setsockopt(ClientInfo.ClientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&OutputTimeOut, sizeof(OutputTimeOut));

			// Initial Check
			int is_received = 0;
			memset(&ReceivedData, 0, sizeof(ReceivedData)); // Clear the receive Buffer(ReceivedDatafer)			
			is_received = recvfrom(ClientInfo.ClientSocket, ReceivedData, sizeof(ReceivedData), 0, (sockaddr*)&ClientInfo.ClientAddress, &ClientLength);

			if ((is_received == 0) || (WSAGetLastError() > 0))// When the size is 0
			{
				Essenbp::WriteLogToFile("Error No Data Received From recvfrom() in ReceiveDataThreadIPv4TCP In: NetworkWrapper!");
				Essenbp::WriteLogToFile("\n Error recvfrom() Failed with Error " + std::to_string(WSAGetLastError()) + " in ReceiveDataThreadIPv4TCP In: NetworkWrapper!");
				bool IsSuccessful = false;
				ClientsList->RemoveClientIPv4(ClientInfo.ClientNumber, IsSuccessful);
				if (!IsSuccessful)
				{
					Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv4 Failed in ReceiveDataThreadIPv4TCP In: NetworkWrapper!");
				}
				return;
			}

			while (true)
			{
				is_received = 0;
				memset(&ReceivedData, 0, sizeof(ReceivedData)); // Clear the receive Buffer(ReceivedDatafer)			
				is_received = recvfrom(ClientInfo.ClientSocket, ReceivedData, sizeof(ReceivedData), 0, (sockaddr*)&ClientInfo.ClientAddress, &ClientLength);

				while (!ContinueInputThread)//PENDING Set Atomic Boolean
				{
					std::cout << '\n' << "Continue Input Thread is Executed ";
					//Infinite Loop here when ServerInputIsPaused
				}

				if ((is_received == 0) || (WSAGetLastError() > 0))// When the size is 0
				{
					Essenbp::WriteLogToFile("Error No Data Received From recvfrom() in ReceiveDataThreadIPv4TCP In: NetworkWrapper!");
					Essenbp::WriteLogToFile("\n Error recvfrom() Failed with Error " + std::to_string(WSAGetLastError()) + " in ReceiveDataThreadIPv4TCP In: NetworkWrapper!");
					bool IsSuccessful = false;
					ClientsList->RemoveClientIPv4(ClientInfo.ClientNumber, IsSuccessful);
					if (!IsSuccessful)
					{
						Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv4 Failed in ReceiveDataThreadIPv4TCP In: NetworkWrapper!");
					}
					return;
				}

				//PENDING Set Message buffer

				//memset(&ConvertedData, 0, sizeof(ConvertedData));
				ConvertedData = SocketCharDataToASCII(ReceivedData, sizeof(ReceivedData));
				OurReturnValue = DetermineTypeOfCommandFromClientAndProcessThem(ClientSocket, Client, ConvertedData, MultipurposeVariable, ClientNumberInServer);// This is the Functions which Determines what the Command is Then Processes
				if (OurReturnValue > 1)// 0 is Unrecognized, 1 is Automated, From 2 and so on Return Value is Valid
				{
					CharactersInServer[ClientNumberInServer].ClientDataSendPendingCommand.push_back(OurReturnValue);
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
					ClientOrderIPv4* ClientInfo;
					SOCKET ClientSocket;
					sockaddr_in ClientAddress;
					int ClientAdderStructLength = sizeof(ClientAddress);
					while (true)
					{
						// Wait for message
						listen(SocketIPv4, MaximumBackLogConnectionsTCP);
						ClientSocket = accept(SocketIPv4, (sockaddr*)&ClientAddress, &ClientAdderStructLength);

						if (WSAGetLastError() > 0)
						{
							Essenbp::WriteLogToFile("\n Error accept() Failed with Error " + std::to_string(WSAGetLastError()) + " in ListenForTCPConnectionIPv4 In: NetworkWrapper!");
							continue;
						}

						ClientsList->AddClientIPv4(ClientSocket, ClientAddress, IsSuccessful);
						if (!IsSuccessful)
						{
							Essenbp::WriteLogToFile("\n Error ClientList::AddClientIPv4 Failed in ListenForTCPConnectionIPv4 In: NetworkWrapper!");
							if (ClientSocket != NULL)
							{
								closesocket(ClientSocket);
							}
						}
						else
						{
							ClientsList->GetClientIPv4((ClientsList->GetTotalNumberOfIPv4Clients() - 1), &ClientInfo, IsSuccessful);
							if (!IsSuccessful)
							{								
								Essenbp::WriteLogToFile("\n Error ClientList::AddClientIPv4 Failed in ListenForTCPConnectionIPv4 In: NetworkWrapper!");
								ClientsList->RemoveClientIPv4((ClientsList->GetTotalNumberOfIPv4Clients() - 1), IsSuccessful);
								if (!IsSuccessful)
								{
									Essenbp::WriteLogToFile("\n Error ClientList::RemoveClientIPv4 Failed in ListenForTCPConnectionIPv4 In: NetworkWrapper!");
									if (ClientSocket != NULL)
									{
										closesocket(ClientSocket);
									}
								}
							}
							else
							{
								std::thread(ReceiveDataThreadIPv4TCP, *ClientInfo).detach();// Seperate Independent Thread No Outside Connection
							}
						}
					}
				}				
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error ListenForTCPConnectionIPv4 Failed In: NetworkWrapper!");
			}
		}

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
						ClientOrderIPv6* ClientOrderIPv6prt = nullptr;
						ClientsList->GetClientIPv6(ClientNumber, &ClientOrderIPv6prt, IsSuccessful);
						if(IsSuccessful)
						{
							if (TrueForTCPFalseForUDP)
							{
								SendDataTCP(&(ClientOrderIPv6prt->ClientSocket), DataAndSize, IsSuccessful);
							}
							else
							{
								SendDataUDP(&(ClientOrderIPv6prt->ClientAddress), DataAndSize, IsSuccessful);
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv6() Failed in SendData In: NetworkWrapper!");
						}
					}
					else
					{
						ClientOrderIPv4* ClientOrderIPv4prt = nullptr;
						ClientsList->GetClientIPv4(ClientNumber, &ClientOrderIPv4prt, IsSuccessful);
						if (IsSuccessful)
						{
							if (TrueForTCPFalseForUDP)
							{
								SendDataTCP(&(ClientOrderIPv4prt->ClientSocket), DataAndSize, IsSuccessful);
							}
							else
							{
								SendDataUDP(&(ClientOrderIPv4prt->ClientAddress), DataAndSize, IsSuccessful);
							}
						}
						else
						{
							Essenbp::WriteLogToFile("\n Error ClientOrderList::GetClientIPv4() Failed in SendData In: NetworkWrapper!");
						}
					}
				}
			}

			if (!IsSuccessful)// For the safe of readability
			{
				Essenbp::WriteLogToFile("\n Error SendData Failed In: NetworkWrapper!");
			}
		}

		~NetworkWrapper()
		{
			Essenbp::WriteLogToFile("\n Destructing NetworkWrapper!");
			if (IsConstructionSuccessful)
			{
				if (SocketIPv4 != NULL)
				{
					closesocket(SocketIPv4);
				}
				if (SocketIPv6 != NULL)
				{
					closesocket(SocketIPv6);
				}
				IsConstructionSuccessful = false;
			}
		}
	};
};


#endif // !NETWORKING_WRAPPER_BY_PUNAL