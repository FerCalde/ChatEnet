// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Net/factoryenet.h"

namespace
{
	const int s_iPort = 65785;
	const int s_iMaxClients = 10;
}

int main()
{
	//Create Factory and Server
	Net::CFactory* pFactory = new Net::CFactoryEnet();
	Net::CServer* pServer = pFactory->buildServer();

	//Init Server
	pServer->init(s_iPort, s_iMaxClients);
	//Receive Data-packages
	std::vector<Net::CPacket*> aPackets; //ptr a los paquetes que recibe y almacena del service //No corta el flujo.

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

			}break;
			case Net::DATA:
			{

			}break;
			case Net::DISCONNECTION:
			{

			}break;
			
			}
			
			delete pPacket;

			/*Net::CPacket* pPacket = *it;
			std::cout << "Packet received: " << pPacket->getData() << std::endl;
			delete pPacket;*/
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

