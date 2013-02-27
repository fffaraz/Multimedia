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

#ifndef __TRANSCEIVER_H__INCLUDED__
#define __TRANSCEIVER_H__INCLUDED__

#include "audio.h"
#include "device.h"
#include "audio.h"
#include "transport.h"
#include "codec.h"
#include "codecfactory.h"

#include <vector>
#include <cc++/address.h>
#include <string>
#include <map>

#include <portaudio.h>
#include <alsa/asoundlib.h>
#include <ccrtp/rtp.h>

using namespace std;
using namespace ost;

#define RING_BUFFER_SIZE	480


namespace agh {

#define SAMPLE_TYPE	paInt16
typedef short sampleType;

class Transceiver;
//class Audio;

class Transmitter : public ost::Thread, TimerPort {
	Transceiver *t;
	
	RingBuffer *outBuffer;
public:
	Transmitter(Transceiver *t);
	~Transmitter();
	
	void run();
};

class Receiver : public ost::Thread, TimerPort {
	Transceiver *t;
public:
	Receiver(Transceiver *t);
	~Receiver();
	
	void run();
};

class Transceiver {
	CodecFactory *cf;
	
	Transmitter *transmitter;
	Receiver *receiver;
	
public:
	Audio *audio;
	Transport *transport;
	Codec* codec;
	
	int framesPerBuffer;
	int jitterMult;
	int packetSize;
	
	Transceiver();
	~Transceiver();
	void setAudio(Audio* audio);
	void setTransport(Transport* transport);
	vector<IDevice*> getAvailableInputDevices() const;
	vector<IDevice*> getAvailableOutputDevices() const;
	int setInputDevice(const IDevice& dev);
	int setOutputDevice(const IDevice& dev);
	int setInputDevice(const string& dev);
	int setOutputDevice(const string& dev);
	int setCodec(int codec);
	int setLocalEndpoint(const IPV4Address& addr, int port);
	int setRemoteEndpoint(const IPV4Address& addr, int port);
	
	int start();
	int stop();
	
};

} /* namespace agh */

#endif /* __TRANSCEIVER_H__INCLUDED__ */
