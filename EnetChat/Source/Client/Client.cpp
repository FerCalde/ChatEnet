// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Net/factoryenet.h"
#include "Net/buffer.h"


static bool bDebugMsg = true;

void printLogs(bool* _bDebugMsg, const char* _msg);
void printLogs(const char* _msg);
void printReceiveMsg(const char* _msg);

namespace
{
	const int s_iPort = 65785;
	const int s_iMaxClients = 10;
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

	std::cout << "Client: Write your Messages\n";

	//Receive Data-packages
	std::vector<Net::CPacket*> aPackets; //ptr a los paquetes que recibe y almacena del service //No corta el flujo.
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
				oData.read(sMessage, oData.getSize());
				
				sMessage[oData.getSize()] = '\0';
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
		pClient->sendData(pConnection, sMessage, strlen(sMessage), 0, true);

	} while (strcmp(sMessage, "closeserver") != 0);







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