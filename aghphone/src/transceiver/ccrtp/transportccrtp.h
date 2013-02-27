/*
 * Copyright (C) 2008  Mateusz Kramarczyk <kramarczyk (at) gmail (dot) com>
 * Copyright (C) 2008  Tomasz Kijas <kijasek (at) gmail (dot) com>
 * Copyright (C) 2008  Tomir Kryza <tkryza (at) gmail (dot) com>
 * Copyright (C) 2008  Maciej Kluczny <kluczny (at) fr (dot) pl>
 * Copyright (C) 2008  AGH University of Science and Technology <www.agh.edu.pl>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TRANSPORTCCRTP_H__INCLUDED__
#define __TRANSPORTCCRTP_H__INCLUDED__

//#include "ringbuffer.h"
#include "msgbuffer.h"
#include "transport.h"
#include "aghrtpsession.h"
#include "transceiver.h"
#include <cc++/address.h>
#include <ccrtp/rtp.h>

using namespace std;

namespace agh {

class TransportCCRTP : public Transport {
//	Transceiver *t;
	
	uint32 timestamp;
	MsgBuffer *sendBuffer;
	
	IPV4Address localAddress;
	int localPort;
	IPV4Address remoteAddress;
	int remotePort;
	
	AghRtpSession *socket;
	
	int framesPerPacket;
	int frameSize;
	
	FILE *out1, *out2;
public:
	TransportCCRTP();
	~TransportCCRTP();
	
	virtual void setParams(int framesPerPacket, int frameSize);
	
	virtual int setLocalEndpoint(const IPV4Address& addr, int port);
	virtual int setRemoteEndpoint(const IPV4Address& addr, int port);
	
	virtual int start();
	virtual int stop();
	
	virtual void send(char* src, int size);
	virtual int recv(char* dest);
	
	virtual void flush();
};

} /* namespace agh */

#endif
