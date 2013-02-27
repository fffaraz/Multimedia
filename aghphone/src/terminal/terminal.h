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

#ifndef __TERMINAL_H__INCLUDED__
#define __TERMINAL_H__INCLUDED__

#include <Ice/Ice.h>
#include <Ice/Identity.h>

#include <cc++/thread.h>

#include "icecommon.h"
#include "transceiver.h"
#include "master.h"
#include "states.h"
#include "globals.h"
#include "iface.h"

namespace agh {

class MasterCallbackImpl : public IMasterCallback {
protected:
	agh::IMaster *master;
public:
	MasterCallbackImpl(agh::IMaster *master);

    virtual void remoteTryConnectAck(const CallParametersResponse& response, const ::Ice::Current& curr);
    virtual void remoteTryConnectNack(const ::Ice::Current& curr);
};

class Terminal : public ISlave, public IMaster {
protected:
	const static string adapterName;
	const static string adapterCallbackName;
	const static string remoteTerminalName;
	const static int defaultIcePort;
	const static int defaultRtpPort;

	int codec;

	int remoteRTPPort;
	int localRTPPort;

	int remoteIcePort;
	int localIcePort;

	IPV4Address *localAddr;
	IPV4Address *remoteAddr;

	int currentState;

	Ice::CommunicatorPtr ic;
	Ice::ObjectAdapterPtr adapter;

	IUICallback* localCallback;
	Transceiver* transceiver;

	ISlavePrx remoteTerminal;
	IMasterCallbackPtr masterCallbackPtr;
	IMasterCallbackPrx masterCallbackPrx;

public:
	Terminal(int lIcePort = defaultIcePort);
	virtual ~Terminal();

	virtual bool isConnected() const; // TODO getState
	virtual void connect(const IPV4Address& addr, int remoteIcePort);
	virtual void disengage();
	virtual void setLocalRtpPort(int port);
	virtual int getLocalRtpPort() const;
	int getLocalIcePort() { return localIcePort; }
	void setLocalIcePort(const int port) { localIcePort = port; }
	virtual int getDestinationRtpPort() const;
	virtual const IPV4Address *getRemoteHost() const;
	virtual const IPV4Address *getLocalHost() const;
	virtual int startTransmission();
	virtual void registerCallback(IUICallback *callback);
	virtual void unregisterCallback();
	virtual void setTransceiver(Transceiver *transceiver);
	virtual void unsetTransceiver();
	IMasterCallbackPrx getMasterCallback() { return masterCallbackPrx; }

	void setAudioCodec(int codecID) { codec = codecID; }
	int getAudioCodec() { return codec; }

    virtual TerminalCapabilities remoteGetCapabilities(const ::Ice::Current& curr);

    virtual void remoteTryConnect(const ::agh::CallParameters&, const ::Ice::Identity& ident, const ::Ice::Current& curr);
    virtual void remoteStartTransmission(const ::Ice::Current& curr);
    virtual void remoteDisengage(const ::Ice::Current& curr);

    virtual void onACK(const ::agh::CallParametersResponse&);
    virtual void onNACK();
private:
	void changeState(int newState);
};

} /* namespace */

#endif /* __TERMINAL_H__INCLUDED__ */

