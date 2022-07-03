//---------------------------------------------------------------------------
// connectionENet.h
//---------------------------------------------------------------------------

/**
 * @file connectionENet.h
 *
 * Representa una conexión con un peer implementada con ENet.
 *
 *
 * @author Juan A. Recio-García, David Llansó
 * @date Octubre, 2006
 */

#ifndef __CONNECTIONENET_H
#define __CONNECTIONENET_H

#define DEBUG_CONECTION 1

#include "connection.h"
#include "enet.h"

namespace Net {

class CConnectionENet: public CConnection{

public:
	CConnectionENet():_peer(0), _id(0xFFFFFFFF){}

	int getAddress();
	short getPort();

	void setId(NetID id);

	NetID getId();

    void setENetPeer(ENetPeer* p);

	ENetPeer* getENetPeer();
private:
	ENetPeer* _peer;
	NetID _id;
};


} // namespace Net

#endif // __CONNECTION_ENET_H