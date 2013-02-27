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

#include "audio.h"
#include "transport.h"
#include "transceiver.h"
#include "codec.h"
#include <cc++/address.h>

using namespace std;
using namespace ost;

namespace agh {

Transceiver::Transceiver()
{
	transport = NULL;
	audio = NULL;

	transmitter = new Transmitter(this);
	receiver = new Receiver(this);

	cf = new CodecFactory();
	codec = NULL;
}

Transceiver::~Transceiver()
{
	if( transport ) delete transport;
	if( audio ) delete audio;
	if( cf ) delete cf;
}

void Transceiver::setAudio(Audio* audio)
{
	this->audio = audio;
}

void Transceiver::setTransport(Transport* transport)
{
	this->transport = transport;
}

vector<IDevice*> Transceiver::getAvailableInputDevices() const
{
	return audio->getAvailableInputDevices();
}

vector<IDevice*> Transceiver::getAvailableOutputDevices() const
{
	return audio->getAvailableOutputDevices();
}

int Transceiver::setInputDevice(const IDevice& dev)
{
	return audio->setInputDevice(dev);
}

int Transceiver::setOutputDevice(const IDevice& dev)
{
	return audio->setOutputDevice(dev);
}

int Transceiver::setInputDevice(const string& dev)
{
	return audio->setInputDevice(dev);
}

int Transceiver::setOutputDevice(const string& dev)
{
	return audio->setOutputDevice(dev);
}

int Transceiver::setCodec(int codec)
{
	this->codec = cf->getCodec(codec);

	this->framesPerBuffer = this->codec->getFrameCount();
	this->jitterMult = 5;
	this->packetSize = this->codec->getFrameSize();

	transport->setParams(this->framesPerBuffer, this->packetSize);

	if( this->codec == NULL )
		return -1;

	return 0;
}

int Transceiver::setLocalEndpoint(const IPV4Address& addr, int port)
{
	return transport->setLocalEndpoint(addr, port);
}

int Transceiver::setRemoteEndpoint(const IPV4Address& addr, int port)
{
	return transport->setRemoteEndpoint(addr, port);
}


int Transceiver::start()
{
	transport->start();
	audio->start();

	transmitter->start();
	receiver->start();

	return 0;
}

int Transceiver::stop()
{
	audio->stop();
	transport->stop();

	return 0;
}

Transmitter::Transmitter(Transceiver *t)
{
	this->t = t;
}

Transmitter::~Transmitter()
{
	delete outBuffer;
}

void Transmitter::run()
{
	outBuffer = new RingBuffer(t->framesPerBuffer*200, t->packetSize);

	char buf[2048];

	TimerPort::setTimer(10);

	while( 1 ) {
		t->audio->read();
		bool canRead = t->audio->getData((void*)buf, t->framesPerBuffer);

		if( canRead ) {
			char bufferenc[2048];
			int elen = t->codec->encode(bufferenc, buf);
			t->transport->send(bufferenc, elen);
		}

		t->transport->flush();

		Thread::sleep(TimerPort::getTimer());
		TimerPort::incTimer(10);
	}
}

Receiver::Receiver(Transceiver *t)
{
	this->t = t;
}

Receiver::~Receiver()
{
}

void Receiver::run()
{
	TimerPort::setTimer(10);

	while(1) {
		char buf[2048];
		long size = t->transport->recv(buf);

		if( size > 0 ) {
			char dbuf[2048];

			int dlen = t->codec->decode(dbuf, buf, size);
			t->audio->putData(dbuf, dlen/t->packetSize);
		}

		t->audio->flush();

		Thread::sleep(TimerPort::getTimer());
		TimerPort::incTimer(10);
	}
}

}
