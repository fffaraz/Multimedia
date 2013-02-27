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

#ifndef __AUDIO_H__INCLUDED__
#define __AUDIO_H__INCLUDED__

#include "ringbuffer.h"
#include "device.h"
#include <vector>
#include <string>

using namespace std;

namespace agh {

class Transceiver;
	
class Audio {
public:
	virtual ~Audio() {}
	
	virtual void setTransceiver(Transceiver *t) = 0;
	virtual vector<IDevice*> getAvailableInputDevices() const = 0;
	virtual vector<IDevice*> getAvailableOutputDevices() const = 0;
	virtual int setInputDevice(const IDevice& dev) = 0;
	virtual int setOutputDevice(const IDevice& dev) = 0;
	virtual int setInputDevice(const string& dev) = 0;
	virtual int setOutputDevice(const string& dev) = 0;
	
	/*
	 * sets size of a data chunk that will be read from buffer with getData()
	 * @param size size of the packet [B]  
	 */
	virtual void setPacketSize(long size) = 0;
	
	/*
	 * sets a value that will be multiplied by a packet size (see setPacketSize())
	 * to form a minimum number of bytes in the buffer that will cause the audio thread
	 * to invoke callback on the transceiver
	 * @param m the multiplier
	 */
	virtual void setJitterMultiplier(int m) = 0;
	
	virtual int start() = 0;
	virtual int stop() = 0;
	
	/*
	 * gets data from audio input, it will copy data from the input buffer to the
	 * memory region pointed by dest, size of the requested data is set with setPacketSize
	 * @param[out] dest pointer to the memory region to which data is to be copied
	 */
	virtual bool getData(void* dest, long size) = 0;
	
	/*
	 * puts data into the output buffer
	 * @param[in] src pointer to the memory region from which data is to be read
	 * @param size size of the data [B]
	 */
	virtual void putData(void* src, long size) = 0;
	
	virtual void moveData(RingBuffer* dest, long size) = 0;
	
	virtual void read() = 0;
	virtual void flush() = 0;
};

} /* namespace agh */

#endif
