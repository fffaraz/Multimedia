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

#ifndef __RINGBUFFER_H__INCLUDED__
#define __RINGBUFFER_H__INCLUDED__

#include <stdio.h>

namespace agh {
	
class RingBuffer {
	char* buffer;
	long bufferSize;
	int sampleSize;
	long readyCount;
	long writeCursor, readCursor;
public:
	RingBuffer(long size, int packetSize);
	~RingBuffer();
	
	bool putData(char *data, long size);
	bool putSilence(long size);
	bool getData(char *data, long size);
	bool peekData(char *data, long size);
	bool skipData(long size);
	bool moveData(RingBuffer *dest, long size);
	
	char* getInputPtr() {
		printf("buffersize : %ld, readCursor : %ld, sampleSize : %d\n", bufferSize, readCursor, sampleSize); 
		return buffer+readCursor*sampleSize;
	}
	
	long getReadyCount();
	long getFreeCount();
};	

} /* namespace agh */

#endif /* __RINGBUFFER_H__INCLUDED__ */
