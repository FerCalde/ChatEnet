// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Net/factoryenet.h"
#include "Net/buffer.h"
#include <map>


static bool bDebugMsg = true;

void printLogs(bool* _bDebugMsg, const char* _msg);
void printLogs(const char* _msg);
void printReceiveMsg(const char* _msg);

namespace
{
	const int s_iPort = 65785;
	const int s_iMaxClients = 10;
	enum class EMessageType
	{
		SetID,
		SetName,
		Message
	};
}



int main()
{
	//Create Factory and Client
	Net::CFactoryEnet* pFactory = new Net::CFactoryEnet();
	Net::CClient* pClient = pFactory->buildClient();

	//Init Client
	pClient->init(1);
	std::cout << "Write the Server IP: ";

	char sMessage[128]{ '\0' };
	fgets(sMessage, 127, stdin);
	char* sEndLine = strchr(sMessage, '\n');
	*sEndLine = '\0';

	//Conect to a Server with the given IP
	Net::CConnection* pConnection = pClient->connect(sMessage, s_iPort, 0);

	printLogs("Client: Connected to Server");

	Net::NetID m_xID = 0;
	std::map<Net::NetID, char*> m_tIDName;

	//Receive Data-packages
	std::vector<Net::CPacket*> aPackets; //ptr a los paquetes que recibe y almacena del service //No corta el flujo.

	//Wait until the ID is set
	bool bIDSet = false;
	do
	{
		pClient->service(aPackets);
		for (/*auto&*/Net::CPacket* pPacket : aPackets)
		{

			Net::CBuffer oData;
			oData.write(pPacket->getData(), pPacket->getDataLength());
			oData.reset();
			EMessageType eMessageType;
			oData.read(&eMessageType, sizeof(eMessageType));
			if (eMessageType == EMessageType::SetID)
			{
					oData.read(&m_xID, sizeof(m_xID));
					bIDSet = true;
				/*if (m_xID == 0)
				{
				}*/
			}

			delete pPacket;
		}
		aPackets.clear(); //Clean Msg List


	} while (!bIDSet);

	std::cout << "Client: Your ID is: " << m_xID << "\n\n";

	std::cout << "Write your Nickname: ";
	fgets(sMessage, 127, stdin); //Here the app will wait for the message to be sent. TODO: UPGRADE MultiThreads.
	char* sIDEndLine = strchr(sMessage, '\n');
	*sIDEndLine = '\0';
	EMessageType eMessageType = EMessageType::SetName;
	Net::CBuffer oNameData;
	oNameData.write(&eMessageType, sizeof(eMessageType));
	oNameData.write(&m_xID, sizeof(m_xID));
	oNameData.write(sMessage, strlen(sMessage));
	
	pClient->sendData(pConnection, oNameData.getData(), oNameData.getSize(), 0, true);
	
	std::cout << "Client: Write your Messages\n";
	do
	{
		//Read Message from Received Package
		pClient->service(aPackets);
		for (Net::CPacket* pPacket : aPackets)
		{
			switch (pPacket->getType())
			{
			case Net::CONNECTION:
			{
				printLogs("Client: Connection Package");
			}break;
			case Net::DISCONNECTION:
			{
				printLogs("Client: Disconnection Package");
			}break;
			case Net::DATA:
			{

				
				//Data Buffer from the Received Packet
				Net::CBuffer oData; //To Read
				oData.write(pPacket->getData(), pPacket->getDataLength());
				oData.reset();

				//Read Message Type and ID
				EMessageType eMessageType;
				oData.read(&eMessageType, sizeof(eMessageType));
				Net::NetID xID;
				oData.read(&xID, sizeof(xID));
				
				//Read the Message Data
				size_t iSize = oData.getSize() - sizeof(xID) - sizeof(eMessageType);
				oData.read(sMessage, iSize);

				sMessage[iSize] = '\0';

				switch (eMessageType)
				{
				case EMessageType::SetName:
				{
					size_t iCopySize = strlen(sMessage) + 1;
					char* sCopy = (char*)malloc(iCopySize);
					strcpy_s(sCopy, iCopySize, sMessage);
					//Name and ID given by Msg read
					//m_tIDName[xClientID] = sMessage;
					m_tIDName[xID] = sCopy;
					std::cout << "\n" << sMessage << " is Connected!\n";
				}break;
				case EMessageType::Message:
				{
					// Make stuff with Message Data Received
					std::cout << "\nClient: " << m_tIDName.at(xID) << "-->" << sMessage << "\n";
					//printReceiveMsg(m_tIDName.at(xClientID), sMessage);
					//printReceiveMsg(sMessage);
				}break;
				}
				
				//Make stuff with Message Data Received
				printReceiveMsg(sMessage);

			}break;
			}

			delete pPacket;
		}

		aPackets.clear(); //Clean Msg List


		//SendMessage to Server
		
		fgets(sMessage, 127, stdin); //Here the app will wait for the message to be sent. TODO: UPGRADE MultiThreads.
		char* sEndLine = strchr(sMessage, '\n');
		*sEndLine = '\0';

		//Create a package to send Message
		//Net::CPacket* oMessagePacket(Net::DATA, )
		//pClient->sendData(pConnection, sMessage, strlen(sMessage), 0, true);
		Net::CBuffer oDataMessage;
		eMessageType = EMessageType::Message;
		oDataMessage.write(&eMessageType, sizeof(eMessageType));
		oDataMessage.write(&m_xID, sizeof(m_xID));
		oDataMessage.write(sMessage, strlen(sMessage));
		pClient->sendData(pConnection, oDataMessage.getData(), oDataMessage.getSize(), 0, true);

	} while (strcmp(sMessage, "exit") != 0);







	//Clean up Client
	pClient->release();
	delete pClient;
	pClient = nullptr;

	delete pFactory;
	pFactory = nullptr;

}

void printLogs(bool* _bDebugMsg, const char* _msg)
{
	if (_bDebugMsg)
	{
		std::cout << "\n\n" << _msg << "\n\n";
	}

}
void printLogs(const char* _msg)
{
	if (bDebugMsg)
	{
		std::cout << "\n\n" << _msg << "\n\n";
	}
}


void printReceiveMsg(const char* _msg)
{
	if (bDebugMsg)
	{
		std::cout << "\Client:\n\tReceived text : " << _msg << "\n";
	}
}