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

#include <cc++/address.h>
#include <ctime>

#include "mixercore.h"
#include "codecfactory.h"
#include "codec.h"
#include "transport.h"

namespace agh {

char sgn(char x) {
	if (x < 0) return -1;
	else if (x == 0) return 0;
	else return 1;
}

MixerCore::MixerCore(map<string, TerminalInfo*>* remoteHosts) {
	this->remoteHosts = remoteHosts;
	this->buffer = new RingBuffer(1024*1024*100, 1);

	this->receiver = new MixerCoreReceiver(remoteHosts, buffer);
	this->transmitter = new MixerCoreTransmitter(remoteHosts, buffer);
	this->transmitter->start();
	this->receiver->start();
}

MixerCoreReceiver::MixerCoreReceiver(map<string, TerminalInfo*>* remoteHosts, RingBuffer *buffer) {
	this->remoteHosts = remoteHosts;
	this->buffer = buffer;
}

MixerCoreTransmitter::MixerCoreTransmitter(map<string, TerminalInfo*>* remoteHosts, RingBuffer *buffer) {
	this->remoteHosts = remoteHosts;
	this->buffer = buffer;
}

struct timeval start_time;

void printTime() {
	struct timeval now;

	gettimeofday(&now, NULL);

	printf("%3ld.%3ld [s] \n", now.tv_sec - start_time.tv_sec, now.tv_usec/1000);
}

void MixerCoreReceiver::run() {

	char *buf = new char[1024*1024*100];
	char* decBuf = new char[1024*1024*100];
	CodecFactory codecFactory;

	TimerPort::setTimer(10);
	while(1) {
		// Get data from all incoming streams
		map<string, TerminalInfo*>::iterator iter;

		// read data
		int index = 0;
		for( iter = remoteHosts->begin(); iter != remoteHosts->end(); iter++ ) {

			TerminalInfo* info = iter->second;
			if (info == NULL) {
				continue;
			}

			Transport *transport = info->transport;

			if (transport == NULL) {
				continue;
			}

			info->readedSize = transport->recv(buf);

			cout << "[" << index << "] trying to read\n";
			if (info->readedSize > 0) {
				// decode
				Codec *codec = codecFactory.getCodec(info->outgoingCodec);
				info->readedSize = codec->decode(decBuf, buf, info->readedSize); // TODO debug return number of bytes written

				info->buf->putData(decBuf, info->readedSize);
				cout << "[" << index << "] packet has been read\n";
			}

			index++;
		}

		Thread::sleep(TimerPort::getTimer());
		TimerPort::incTimer(10);
	}
}

void MixerCoreTransmitter::run() {

	char *buf = new char[1024*1024*100];
	char* encBuf = new char[1024*1024*100];
	CodecFactory codecFactory;
	int packetSize = 320;

	gettimeofday(&start_time, NULL);
	TimerPort::setTimer(20);
	while(1) {

		if (this->buffer->getReadyCount() > packetSize) {
			this->buffer->getData(buf, packetSize);

			// send
			map<string, TerminalInfo*>::iterator iter;
			for( iter = remoteHosts->begin(); iter != remoteHosts->end(); iter++ ) {

				TerminalInfo* info = iter->second;
				if (info == NULL) {
					continue;
				}

				Transport *transport = info->transport;
				if (transport == NULL) {
					continue;
				}

				// encode
				Codec *codec = codecFactory.getCodec(info->incomingCodec);
				codec->encode(encBuf, buf);

				transport->send(encBuf, packetSize);
				transport->flush();
			}

// 			printTime();
		}

		Thread::sleep(TimerPort::getTimer());
		TimerPort::incTimer(20);
	}
}

/**
 *
 */
void MixerCore::run() {

	char** bufs = new char*[10];
	int packetSize = 320;
	int minPackThreshold = 2;
	int count = 0;
	int available = 0;
	char* overallBuf = new char[1024*1024];

	for (int i = 0; i < 10; i++) {
		bufs[i] = new char[1024*1024];
	}

	TimerPort::setTimer(10);
	while(1) {

		// Get data from all incoming streams
		map<string, TerminalInfo*>::iterator iter;

		// read data
		available = 1;
		int index = 0;
		for( iter = remoteHosts->begin(); iter != remoteHosts->end(); iter++ ) {
			TerminalInfo* info = iter->second;
			if (info == NULL || info->buf == NULL) {
				continue;
			}

// 			if (info->buf->getReadyCount() > packetSize*minPackThreshold ) {
// 				cout << "[" << index << "]max threshold, ready: " << info->buf->getReadyCount() << endl;
// 				info->buf->skipData(packetSize*minPackThreshold);
// 				cout << "[" << index << "]max threshold, after: " << info->buf->getReadyCount() << endl;
// 			}

			if (info->buf->getReadyCount() < packetSize*minPackThreshold ) {
				cout << "[" << index << "]too little skipping\n";
				available = 0;
			}
			index++;
		}

		count = 0;
		if (available)  {
			// read data
			for( iter = remoteHosts->begin(); iter != remoteHosts->end(); iter++ ) {
				TerminalInfo* info = iter->second;
				if (info == NULL || info->buf == NULL) {
					continue;
				}
				if (info->buf->getReadyCount() > packetSize) {
					info->buf->getData(bufs[count],packetSize);
					count++;
				}
			}
		}

		if (count > 0) {
//			 mix original solu]tion
//			for (int i = 0; i < packetSize; i++) {
//				overallBuf[i] = 0;
//				int pCount = count;
//				for (int j = 0; j < count; j++) {
////					if (ISQUIET(bufs[j][i])) pCount--;
////					else
//						overallBuf[i] += bufs[j][i];
//				}
//				if (pCount != 0)
//					overallBuf[i] /= pCount;
//			}

			signed short *pOverallBuf = (signed short*)overallBuf;
			signed short **pBufs = (signed short**)bufs;

			
			// This one is likely to work correctly
			for (unsigned int i = 0; i < packetSize / sizeof(short); ++i) {
				pOverallBuf[i] = 0;
				for (int j = 0; j < count; ++j) {
					pOverallBuf[i] += pBufs[j][i] / count;
				}
			}

 			// mix Align-to-Average Weighted AAW
// 			for (int i = 0; i < packetSize; i++) {
// 				overallBuf[i] = 0;
// 				for (int j = 0; j < count; j++) {
// 					overallBuf[i] += bufs[j][i];
// 				}
// 				overallBuf[i] /= count;
// 			}

// 			// mix Align-to-Greatest Weighted AGW
// 			char totalMax = 0;
// 			char mixedMax = 0;
// 			for (int i = 0; i < packetSize; i++) {
// 				overallBuf[i] = 0;
// 				for (int j = 0; j < count; j++) {
// 					if (bufs[j][i] > totalMax) totalMax = bufs[j][i];
// 					overallBuf[i] += bufs[j][i];
// 				}
// 				overallBuf[i] /= count;
// 				if (overallBuf[i] > mixedMax) mixedMax = overallBuf[i];
// 			}
// 			char u = 1; // factor used to adjust the amplitude
// 			for (int i = 0; i < packetSize; i++) {
// 				overallBuf[i] = overallBuf[i]*u*totalMax/mixedMax;
// 			}

// 			// mix Align-to-Self Weighted ASW
// 			for (int i = 0; i < packetSize; i++) {
// 				overallBuf[i] = 0;
// 				char sum = 0;
// 				for (int j = 0; j < count; j++) {
// 					overallBuf[i] += bufs[j][i]*bufs[j][i]*sgn(bufs[j][i]);
// 					sum += abs(bufs[j][i]);
// 				}
// 				overallBuf[i] /= sum;
// 			}

// 			// mix Align-to-Energy Weighted AEW
// 			for (int i = 0; i < packetSize; i++) {
// 				overallBuf[i] = 0;
// 				char sum = 0;
// 				for (int j = 0; j < count; j++) {
// 					overallBuf[i] += bufs[j][i]*bufs[j][i]*bufs[j][i];
// 					sum += abs(bufs[j][i])*abs(bufs[j][i]);
// 				}
// 				if (sum == 0) sum = 1;
// 				overallBuf[i] /= sum;
// 			}

//			for (unsigned int i = 0; i < packetSize / sizeof(signed short); ++i) {
//				int temp = 0;
//				int sum = 0;
//				for (int j = 0; j < count; ++j) {
//					temp += pBufs[j][i] * pBufs[j][i] * pBufs[j][i];
//					sum += pBufs[j][i] * pBufs[j][i];
//				}
//				if (sum < 1) sum = 1;
//				temp /= sum;
//				pOverallBuf[i] = (signed short) (temp);
//			}
			
			buffer->putData(overallBuf, packetSize);
			cout << "putted in buffor" << endl;
		}

		Thread::sleep(TimerPort::getTimer());
		TimerPort::incTimer(10);
	}
}

} // namespace
