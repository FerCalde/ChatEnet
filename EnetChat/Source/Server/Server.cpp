// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Net/factoryenet.h"
#include "Net/buffer.h"

namespace
{
	const int s_iPort = 65785;
	const int s_iMaxClients = 10;
}

static bool bDebugMsg = true;

void printLogs(bool* _bDebugMsg, const char* _msg);
void printLogs(const char* _msg);
void printReceiveMsg(const char* _msg);


int main()
{
	//Create Factory and Server
	Net::CFactory* pFactory = new Net::CFactoryEnet();
	Net::CServer* pServer = pFactory->buildServer();
	
	printLogs("Server: Creating...");

	//Init Server
	pServer->init(s_iPort, s_iMaxClients);
	printLogs("Server: Init...");
	
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
				printLogs("Server: Connection from a Client");
			}break;
			case Net::DATA:
			{
				//Data Buffer from the Received Packet
				Net::CBuffer oData; //To Read
				oData.write(pPacket->getData(), pPacket->getDataLength());
				
				oData.reset();
				oData.read(sMessage, oData.getSize());
				sMessage[oData.getSize()] = '\0';
				
				printReceiveMsg(sMessage);

			}break;
			case Net::DISCONNECTION:
			{
				printLogs("Server: Disconnection from a Client");

			}break;
			
			}
			
			delete pPacket;

		}

		aPackets.clear(); //Clean Msg List

	} while (true);

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