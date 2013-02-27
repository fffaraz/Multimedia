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

#ifndef __AUDIOPA_H__INCLUDED__
#define __AUDIOPA_H__INCLUDED__

#include "device.h"
#include "aghrtpsession.h"
#include "audio.h"
#include "transceiver.h"
#include "devicefactorypa.h"
#include <cc++/address.h>

using namespace std;
using namespace ost;

namespace agh {

class AudioPa : public Audio {
private:
	Transceiver *t;
	
	DeviceFactoryPa* devMgr;
	const IDevice* inputDevice;
	const IDevice* outputDevice;
	
	RingBuffer *outputBuffer;
	RingBuffer *inputBuffer;

	long framesPerBuffer;
	float sampleRate;
	int jitterMult;
	
	PaStream* inputStream, *outputStream, *stream;

	void openStream();
public: 
	AudioPa(Transceiver *t);
	~AudioPa();
	void setTransceiver(Transceiver *t);
	vector<IDevice*> getAvailableInputDevices() const;
	vector<IDevice*> getAvailableOutputDevices() const;
	int setInputDevice(const IDevice& dev);
	int setOutputDevice(const IDevice& dev);
	int setInputDevice(const int id);
	int setOutputDevice(const int id);
	int setInputDevice(const string& name) { return 0; }
	int setOutputDevice(const string& name) { return 0; }

	void setSampleRate(float rate) { sampleRate = rate; }

	/*
	 * sets size of a data chunk that will be read from buffer with getData()
	 * @param size size of the packet [B]  
	 */
	void setPacketSize(long size) { framesPerBuffer = size; }
	
	/*
	 * sets a value that will be multiplied by a packet size (see setPacketSize())
	 * to form a minimum number of bytes in the buffer that will cause the audio thread
	 * to invoke callback on the transceiver
	 * @param m the multiplier
	 */
	void setJitterMultiplier(int m) { jitterMult = m; }
	
	int start();
	int stop();
	
	/*
	 * gets data from audio input, it will copy data from the input buffer to the
	 * memory region pointed by dest, size of the requested data is set with setPacketSize
	 * @param[out] dest pointer to the memory region to which data is to be copied
	 * @param size size in bytes of the data to be put in the dest
	 */
	bool getData(void* dest, long size);
	
	/*
	 * puts data into the output buffer
	 * @param[in] src pointer to the memory region from which data is to be read
	 * @param size size of the data [B]
	 */
	
	void putData(void* src, long size);
	
	void moveData(RingBuffer* dest, long size);
	
	void flush();
	void read();
};

} /* namespace agh */

#endif
