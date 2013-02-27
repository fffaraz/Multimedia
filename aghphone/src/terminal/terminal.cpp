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

/** TODO:
 *  - remove ugly "clog <<" debug method
 *  - add some logging
 *  - ice exception handling
 *  - mutexes
 */

#include <iostream>
#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <cc++/address.h>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include "iface.h"
#include "restypes.h"
#include "globals.h"
#include "terminal.h"
#include "states.h"
#include "tools.h"

using namespace std;
using namespace agh;
using namespace ost;

using namespace log4cxx;
using namespace log4cxx::helpers;

namespace agh {

LoggerPtr logger(Logger::getLogger("MyApp"));

const string Terminal::adapterName("AGHPhoneAdapter");
const string Terminal::adapterCallbackName("AGHPhoneCallbackAdapter");
const string Terminal::remoteTerminalName("Slave");
const int Terminal::defaultIcePort = 24474;
const int Terminal::defaultRtpPort = 5004;

Terminal::Terminal(int lIcePort) :
	remoteRTPPort(0), localRTPPort(defaultRtpPort), localIcePort(lIcePort),
			localAddr(0), remoteAddr(0), currentState(States::DISCONNECTED),
			ic(0), adapter(0), remoteTerminal(0), localCallback(0),
			transceiver(0), codec(AudioCodec::LPC) {

	cout << "[Terminal]Constructor" << endl;

	if ((localIcePort < 1024) || (localIcePort > 32768))
		localIcePort = defaultIcePort;

	ic = Ice::initialize();
	stringstream iceEndpoint;
	iceEndpoint << "default -p " << localIcePort;
	adapter = ic->createObjectAdapterWithEndpoints(adapterName,
			iceEndpoint.str());
	ISlavePtr localTerminal = this;
	adapter->add(localTerminal, ic->stringToIdentity(remoteTerminalName));
	adapter->activate();
}

Terminal::~Terminal() {

	// MOT - throws exception??
	//adapter->deactivate();
	//adapter->destroy();
	//ic->destroy();

	if (localAddr) {
		delete localAddr;
	}
	if (remoteAddr) {
		delete remoteAddr;
	}
}

bool Terminal::isConnected() const {
	return currentState != States::DISCONNECTED;
}

void Terminal::setLocalRtpPort(int port) {
	if ((port < 1024) || (port > 32768)) {
	} else {
		localRTPPort = port;
	}
}

int Terminal::getLocalRtpPort() const {
	return localRTPPort;
}

int Terminal::getDestinationRtpPort() const {
	return remoteRTPPort;
}

const IPV4Address* Terminal::getRemoteHost() const {
	return remoteAddr;
}

const IPV4Address* Terminal::getLocalHost() const {
	return localAddr;
}

// TODO retrieve and choose codec
void Terminal::connect(const IPV4Address& addr, int remoteIcePort) {
	stringstream a;
	a << addr;
	LOG4CXX_DEBUG(logger, string("Terminal::connect(") << a.str() << ", " << remoteIcePort << ")");

	if (currentState != States::DISCONNECTED) {
		LOG4CXX_INFO(logger, "Already connected");
		throw TerminalBusyException();
	}

	if (!addr.isInetAddress()) {
		LOG4CXX_ERROR(logger, "Incorrect address: " << a.str());
		//TODO: another Exception
		throw VoipException();
	}

	if ((remoteIcePort < 1024) || (remoteIcePort > 32768)) {
		LOG4CXX_ERROR(logger, "Incorrect or out of range port number: " << remoteIcePort);
		//TODO: Another Exception
		throw VoipException();
	}

	if (remoteAddr)
		delete remoteAddr;
	remoteAddr = new IPV4Address(addr);
	this->remoteIcePort = remoteIcePort;
	localAddr = new IPV4Address("0.0.0.0");

	stringstream remoteEndpoint;
	remoteEndpoint << remoteTerminalName << ":default";
	remoteEndpoint << " -h " << remoteAddr->getAddress();
	remoteEndpoint << " -p " << remoteIcePort;

	LOG4CXX_DEBUG(logger, "Remote Endpoint: \"" << remoteEndpoint.str() << "\"");

	LOG4CXX_DEBUG(logger, "Accessing remote object");
	Ice::ObjectPrx base = ic->stringToProxy(remoteEndpoint.str());
	remoteTerminal = ISlavePrx::checkedCast(base);
	LOG4CXX_DEBUG(logger, "Successfully received remote object");

	LOG4CXX_DEBUG(logger, "Calling remote site");

	// TODO retrieve codecs and choose the best
	CallParameters params;
	params.masterRtpPort = localRTPPort;
	ICodec codecOut, codecIn;
	codecOut.id = codec;
	codecIn.id = codec;
	params.outgoingCodec = codecOut;
	params.incomingCodec = codecIn;

	masterCallbackPtr = new MasterCallbackImpl((IMaster*) this);
	Ice::ObjectAdapterPtr tmpAdapter;
	tmpAdapter = ic->createObjectAdapter("");
	Ice::Identity tmpIdentity;
	tmpIdentity.name = IceUtil::generateUUID();
	tmpIdentity.category = "";

	tmpAdapter->add(masterCallbackPtr, tmpIdentity);
	tmpAdapter->activate();
	remoteTerminal->ice_getConnection()->setAdapter(tmpAdapter);

	this->changeState(States::ACTIVE_CONNECTED);
	remoteTerminal->remoteTryConnect(params, tmpIdentity);
}

void Terminal::changeState(int newState) {
	stringstream a;
	LOG4CXX_DEBUG(logger, "Terminal::changeState()");

	if (this->localCallback != 0) {
		localCallback->onStateTransition(currentState, newState, *localAddr);
	}

	a << string("State transition [") << this->currentState << " -> " << newState << "]";
	LOG4CXX_DEBUG(logger, a.str());
	this->currentState = newState;
}

int Terminal::startTransmission() {
	LOG4CXX_DEBUG(logger, "Terminal::startTransmission()");
	if (transceiver) {
		if (currentState != States::ACTIVE_CONNECTED) {
			LOG4CXX_ERROR(logger, "Not connected or transmission already set");
			return -1;
		}
		//TODO: set codec
		transceiver->setCodec(codec);
		transceiver->setLocalEndpoint(*localAddr, localRTPPort);
		transceiver->setRemoteEndpoint(*remoteAddr, remoteRTPPort);

		stringstream a;
		a << "Starting transceiver :: " << *localAddr << ":" << localRTPPort << " <-> " << *remoteAddr << ":" << remoteRTPPort;
		LOG4CXX_DEBUG(logger, a.str());

		int res = transceiver->start();
		LOG4CXX_DEBUG(logger, "Local transceiver started");


		LOG4CXX_DEBUG(logger, "Transition to ACTIVE_OPERATIONAL state");
		changeState(States::ACTIVE_OPERATIONAL);

		return 0; //TODO: change error mechanism
	}
	LOG4CXX_ERROR(logger, "No transceiver");
	return -1; //TODO: change error mechanism
}

//TODO: stop transceiver
void Terminal::disengage() {
	LOG4CXX_DEBUG(logger, "Terminal::disconnect()");
	if (currentState != States::DISCONNECTED) {
		// TODO: stop trasmission (Transceiver component needed first
		changeState(States::DISCONNECTED);
	}
	changeState(States::DISCONNECTED);
}

void Terminal::registerCallback(agh::IUICallback* callback) {
	localCallback = callback;
}

void Terminal::unregisterCallback() {
	localCallback = 0;
}

void Terminal::setTransceiver(agh::Transceiver* trans) {
	transceiver = trans;
}

void Terminal::unsetTransceiver() {
	transceiver = 0;
}

TerminalCapabilities Terminal::remoteGetCapabilities(const ::Ice::Current& curr) {

}

void Terminal::remoteTryConnect(const ::agh::CallParameters& params,
		const ::Ice::Identity& ident, const ::Ice::Current& curr) {
	stringstream a;
	LOG4CXX_DEBUG(logger, string("Terminal::remoteTryConnect()"));
	masterCallbackPrx
			= IMasterCallbackPrx::uncheckedCast(curr.con->createProxy(ident));

	if (localAddr) {
		delete localAddr;
	}
	if (remoteAddr) {
		delete remoteAddr;
	}
	remoteAddr = new IPV4Address(getRemoteAddressFromConnection(curr.con));
	localAddr = new IPV4Address(getLocalAddressFromConnection(curr.con));

	remoteRTPPort = params.masterRtpPort;
	codec = params.incomingCodec.id;
// 	codecOut = params.outgoingCodec.id;

	changeState(States::PASSIVE_CONNECTED);
}

void Terminal::remoteStartTransmission(const ::Ice::Current& curr) {
	LOG4CXX_DEBUG(logger, string("Terminal::remoteStartTransmission()"));

	if (this->currentState != States::PASSIVE_CONNECTED) {
		LOG4CXX_DEBUG(logger, string("Terminal::remoteStartTransmission() bad state"));
	} else {
		changeState(States::PASSIVE_OPERATIONAL);

		stringstream a;
		a << "Starting passive transceiver :: " << *localAddr << ":" << localRTPPort << " <-> " << *remoteAddr << ":" << remoteRTPPort << " using codec: " << codec;

		transceiver->setCodec(codec); //dummy codec
		transceiver->setLocalEndpoint(*localAddr, localRTPPort);
		transceiver->setRemoteEndpoint(*remoteAddr, remoteRTPPort);
		transceiver->start();

		LOG4CXX_DEBUG(logger, string("Terminal::remoteStartTransmission() transmission started"));
	}
}

//TODO: correct implementation
void Terminal::remoteDisengage(const ::Ice::Current& curr) {
	changeState(States::DISCONNECTED);
}

void Terminal::onACK(const ::agh::CallParametersResponse& param) {
	LOG4CXX_DEBUG(logger, string("Terminal::onACK()"));

	if (this->currentState != States::ACTIVE_CONNECTED) {
		LOG4CXX_DEBUG(logger, string("Terminal::onACK() bad state"));
	} else {
		remoteRTPPort = param.slaveRtpPort;

		cout << "XXXX:" << remoteRTPPort << endl;
		if (remoteRTPPort <= 1024 || remoteRTPPort >= 32765) {
			// 			throw VoipException(); TODO
		}

		stringstream a;
		a << "Terminal::onACK() " << "localH: " << *localAddr << " localP:"
				<< localRTPPort << " remoteH: " << *remoteAddr << " remoteP: "
				<< remoteRTPPort;
		LOG4CXX_DEBUG(logger, a.str());

		// perform connection
		this->startTransmission();
		remoteTerminal->remoteStartTransmission();
		LOG4CXX_DEBUG(logger, string("Terminal::onACK() starting transmission"));
		//

		cout << "Before change state\n";
		changeState(States::ACTIVE_OPERATIONAL);
		cout << "After change state\n";
	}
}

void Terminal::onNACK() {
	cout << "onNAck received\n";
}

MasterCallbackImpl::MasterCallbackImpl(IMaster *master) :
	master(master) {
}

void MasterCallbackImpl::remoteTryConnectAck(
		const ::agh::CallParametersResponse& param, const ::Ice::Current& curr) {
	master->onACK(param);
}

void MasterCallbackImpl::remoteTryConnectNack(const ::Ice::Current& curr) {
	master->onNACK();
}

} /* namespace agh */

