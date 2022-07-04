// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Net/factoryenet.h"
#include "Net/buffer.h"
#include <map>

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

static bool bDebugMsg = true;

void printLogs(bool* _bDebugMsg, const char* _msg);
void printLogs(const char* _msg);
void printReceiveMsg(const char* _msg);
void printReceiveMsg(const char* _IDName, const char* _msg);


int main()
{
	//Create Factory and Server
	Net::CFactory* pFactory = new Net::CFactoryEnet();
	Net::CServer* pServer = pFactory->buildServer();

	printLogs("Server: Creating...");

	//Init Server
	pServer->init(s_iPort, s_iMaxClients);
	printLogs("Server: Init...");

	Net::NetID m_xNextID = 0; //ID for next Client that Connects. '0' is reserved for Server
	std::map<Net::NetID, char*> m_tIDName;

	//Receive Data-packages
	std::vector<Net::CPacket*> aPackets; //ptr a los paquetes que recibe y almacena del service //No corta el flujo.
	char sMessage[128]{ '\0' };

	do
	{
		pServer->service(aPackets);
		//Iterator for the Msg Received
		for (std::vector<Net::CPacket*>::iterator it = aPackets.begin(); it != aPackets.end(); ++it)
		{
			Net::CPacket* pPacket = *it;
			switch (pPacket->getType())
			{

			case Net::CONNECTION:
			{
				printLogs("Server: Connection from a Client\nSending to the Client the ID");

				EMessageType eType = EMessageType::SetID;

				Net::CBuffer oData;
				oData.write(&eType, sizeof(eType));
				++m_xNextID;
				oData.write(&m_xNextID, sizeof(m_xNextID));

				Net::CPacket oIDPacket(Net::DATA, oData.getData(), oData.getSize(), pPacket->getConnection(), 0);
				pServer->sendData(pPacket->getConnection(), oIDPacket.getData(), oIDPacket.getDataLength(), 0, true);
				//Net::CPacket oIDPacket(Net::DATA, (Net::byte*)&eType, sizeof(eType), pPacket->getConnection(), 0);
				//pServer->sendData(pPacket->getConnection(), oData.getData(), oData.getSize(), 0, true);

			}break;
			case Net::DATA:
			{
				//Data Buffer from the Received Packet
				Net::CBuffer oData; //To Read
				oData.write(pPacket->getData(), pPacket->getDataLength());
				oData.reset();

				//Msg ever receive 1º the Type, 2º the ID from the owner, 3º The Message send (the Data-buffer container) 
				EMessageType eMessageType;
				oData.read(&eMessageType, sizeof(EMessageType));
				Net::NetID xClientID;
				oData.read(&xClientID, sizeof(xClientID));
				size_t uMsgSize = oData.getSize() - sizeof(EMessageType) - sizeof(xClientID);
				oData.read(sMessage, uMsgSize); //Read the rest of Buffer.Dont get overflow by reading the space of MsgType or ID
				sMessage[uMsgSize] = '\0';
				//sMessage[oData.getSize()] = '\0';
				// 
				// 
				switch (eMessageType)
				{
				case EMessageType::SetName:
				{
					size_t iCopySize = strlen(sMessage) + 1;
					char* sCopy = (char*)malloc(iCopySize);
					strcpy_s(sCopy, iCopySize, sMessage);
					//Name and ID given by Msg read
					//m_tIDName[xClientID] = sMessage;
					m_tIDName[xClientID] = sCopy;
					std::cout << "\n" << sMessage << " is Connected!\n";

					for (auto rClient : m_tIDName)
					{
						Net::CBuffer oAuxData;
						EMessageType eAuxMessageType = EMessageType::SetName;
						oAuxData.write(&eAuxMessageType, sizeof(eAuxMessageType));
						oAuxData.write(&(rClient.first), sizeof(rClient.first));
						oAuxData.write(&(rClient.second), sizeof(rClient.second));
						pServer->sendData(pPacket->getConnection(), oAuxData.getData(), oAuxData.getSize(),0, true);
	
					}

				}break;
				case EMessageType::Message:
				{
					// Make stuff with Message Data Received
					std::cout << "\n" << m_tIDName.at(xClientID) << "-->" << sMessage << "is Connected!\n";
					//printReceiveMsg(m_tIDName.at(xClientID), sMessage);
					//printReceiveMsg(sMessage);
				}break;
				}

				//Send Message to all Clients
				pServer->sendAll(pPacket->getData(), pPacket->getDataLength(), 0, true);

			}break;
			case Net::DISCONNECTION:
			{
				printLogs("Server: Disconnection from a Client");

			}break;

			}

			delete pPacket;

		}

		aPackets.clear(); //Clean Msg List

	} while (strcmp(sMessage, "exit") != 0);


	//Close and Clean Server & Factory
	pServer->release();
	delete pServer;
	pServer = nullptr;

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
		std::cout << "\nServer:\n\tReceived text : " << _msg << "\n";
	}
}

void printReceiveMsg(const char* _IDName, const char* _msg)
{
	if (bDebugMsg)
	{
		std::cout << "\nServer:\n\tReceived text from: " << _IDName << " -->" << _msg << "\n";
	}

}