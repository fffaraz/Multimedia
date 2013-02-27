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

#ifndef __TRANSPORT_H__INCLUDED__
#define __TRANSPORT_H__INCLUDED__

#include <cc++/address.h>

using namespace std;
using namespace ost;

namespace agh {
	
class Transport {
public:
	virtual ~Transport() {}
	
	virtual void setParams(int framesPerPacket, int frameSize) = 0;

	virtual int setLocalEndpoint(const IPV4Address& addr, int port) = 0;
	virtual int setRemoteEndpoint(const IPV4Address& addr, int port) = 0;
	
	virtual int start() = 0;
	virtual int stop() = 0;
	
	virtual void send(char* src, int size) = 0;
	virtual int recv(char* dest) = 0;
	
	virtual void flush() = 0;
};

} /* namespace agh */

#endif
