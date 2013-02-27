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

#include "transportccrtp.h"
#include "restypes.h"
#include "sequence_number.h"
#include <stdio.h>
#include <sys/types.h>

using namespace std;

namespace agh {

TransportCCRTP::TransportCCRTP()
{
 	sendBuffer = new MsgBuffer(4096, 20);
	framesPerPacket = 0;
	frameSize = 0;

//	out1 = fopen("out1", "w");
//	out2 = fopen("out2", "w");
}

TransportCCRTP::~TransportCCRTP()
{
}

void TransportCCRTP::setParams(int framesPerPacket, int frameSize)
{
	this->framesPerPacket = framesPerPacket;
	this->frameSize = frameSize;
}

int TransportCCRTP::setLocalEndpoint(const IPV4Address& addr, int port)
{
	localAddress = addr;
	localPort = port;

	return 0;
}

int TransportCCRTP::setRemoteEndpoint(const IPV4Address& addr, int port)
{
	remoteAddress = addr;
	remotePort = port;

	return 0;
}

int TransportCCRTP::start()
{
	if( (frameSize <= 0) || (framesPerPacket <= 0)) {
		cout << "Transport fatal : frameSize or framesPerPacket not set" << endl;
		return -1;
	}

	if( (localPort == -1 ) ) {
		cout << "Endpoint error: " << localAddress << ":" << localPort << endl;
		return TransceiverStartResult::LOCAL_ENDPOINT_ERROR;
	}

	if( ( remotePort == -1 ) || ( !remoteAddress) ) {
		printf("error2\n");
		return TransceiverStartResult::REMOTE_ENDPOINT_ERROR;
	}

	socket = new AghRtpSession( IPV4Host(localAddress.getAddress()), localPort );

	//socket->setSchedulingTimeout(10000);
	socket->setExpireTimeout(51);


	if( !socket->addDestination( IPV4Host(remoteAddress.getAddress()), remotePort ) ) {
		/*
		 * TODO: Implement ccrtp connection failure
		 */
		cout << "CCRTP destination connection failure: " << IPV4Host(remoteAddress.getAddress()) << ":" << remotePort << endl;
		return -1;
	}

	socket->setPayloadFormat( StaticPayloadFormat( sptPCMU ) );

	socket->startRunning();

	if( socket->RTPDataQueue::isActive() )
		cout << "active." << endl;
	else
	    cerr << "not active." << endl;

	timestamp = socket->getCurrentTimestamp() + frameSize*framesPerPacket;

	return 0;
}

int TransportCCRTP::stop()
{
	delete socket;

	return 0;
}

void TransportCCRTP::send(char* src, int size)
{
	sendBuffer->putMessage(src, size);
}

int TransportCCRTP::recv(char* dest)
{
	const AppDataUnit* adu;
	adu = socket->getData(socket->getFirstTimestamp());
	long size;
  	if ( (NULL != adu) && ( (size = adu->getSize()) > 0 ) ) {
		char *ptr = (char*) adu->getData();
		for(int i=0;i<size;i++)
			*dest++ = *ptr++;
	} else {
		return 0;
	}
	return size;
}

void TransportCCRTP::flush()
{
	if( sendBuffer->getReadyCount() > 0 ) {
		uint32 current_timestamp = socket->getCurrentTimestamp();
		//printf("%ld %ld\n", current_timestamp, timestamp);
		//fprintf(out1, "%ld\n", current_timestamp);
		//fprintf(out2, "%ld\n", timestamp);
		if(seq32_t(timestamp) <= seq32_t(current_timestamp + framesPerPacket)) {
			socket->setExpireTimeout(frameSize*framesPerPacket * 1000);
			char *buf;
			int length = sendBuffer->peekMessage(&buf);
			socket->putData(timestamp, (unsigned char*)buf, length);
			sendBuffer->skipMessage();
			timestamp += framesPerPacket;
		} else {
			//printf("discarding surplus of audio samples, current timestamp: %ld, timestamp: %ld\n",
				//current_timestamp, timestamp); fflush(stdout);
		}

		if(current_timestamp > timestamp+2000)
			timestamp = socket->getCurrentTimestamp();
	}
	sendBuffer->debug();
}

} /* namespace agh */
