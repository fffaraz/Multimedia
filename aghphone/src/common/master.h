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

#ifndef __MASTER_H__INCLUDED__
#define __MASTER_H__INCLUDED__

#include <cc++/address.h>
#include "../transceiver/transceiver.h"
#include "../ice/icecommon.h"
#include "../ice/states.h"


using namespace ost;

namespace agh {

class IUICallback {
	
public:
	virtual ~IUICallback() {}
	
	/**
	 * if returns false the call is rejected
	 */
	virtual bool onStateTransition(int prevState, int curState, const IPV4Address& addr) = 0;
};

class IMaster {
public:
	virtual ~IMaster() {}
	virtual void connect(const IPV4Address& addr, int icePort) = 0;
	virtual void disengage() = 0;
	virtual bool isConnected() const = 0;
	virtual void setLocalRtpPort(int port) = 0;
	virtual int getLocalRtpPort() const = 0;
	virtual int getDestinationRtpPort() const = 0;
	virtual const IPV4Address *getRemoteHost() const = 0;
	virtual const IPV4Address *getLocalHost() const = 0;
	
	virtual int startTransmission() = 0;
	
	virtual void registerCallback(IUICallback *callback) = 0;
	virtual void unregisterCallback() = 0;
	
	virtual void setTransceiver(Transceiver *transceiver) = 0;
	virtual void unsetTransceiver() = 0;
	
	virtual void onACK(const CallParametersResponse&) = 0;
	virtual void onNACK() = 0;
};

} /* namespace agh */

#endif /* __MASTER_H__INCLUDED__ */
