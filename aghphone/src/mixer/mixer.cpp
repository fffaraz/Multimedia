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
#include <IceUtil/Thread.h>
#include <cc++/address.h>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include "mixer.h"
#include "codecfactory.h"
#include "codec.h"
#include "transportccrtp.h"
#include "globals.h"

using namespace std;
using namespace agh;
using namespace ost;

using namespace log4cxx;
using namespace log4cxx::helpers;

namespace agh {

static LoggerPtr logger(Logger::getLogger("MixerCPP"));

const string Mixer::adapterName("AGHPhoneAdapter");
const string Mixer::adapterCallbackName("AGHPhoneCallbackAdapter");
const string Mixer::remoteMixerName("Slave");
const int Mixer::defaultIcePort = 24474;
const int Mixer::defaultRtpPort = 6014;

class WorkerThread : public IceUtil::Thread {
private:
	Mixer *mixer;
	CallParametersResponse params;
public:
	WorkerThread(Mixer* mixer, CallParametersResponse params);
	virtual void run();
};

WorkerThread::WorkerThread(Mixer* mixer, CallParametersResponse params) {
	this->mixer = mixer;
	this->params = params;
}

void WorkerThread::run() {
	mixer->getMasterCallback()->remoteTryConnectAck(params);
}


Mixer::Mixer(int lIcePort) :
	localRTPPort(defaultRtpPort),
	localIcePort(lIcePort), localAddr(0),
	remoteAddr(0), currentState(States::DISCONNECTED), ic(0), adapter(0) {

	cout << "Constructor of mixer" << endl;

	if ((localIcePort < 1024) || (localIcePort > 32768))
		localIcePort = defaultIcePort;

	ic = Ice::initialize();
	stringstream iceEndpoint;
	iceEndpoint << "default -p " << localIcePort;
	adapter = ic->createObjectAdapterWithEndpoints(adapterName,
			iceEndpoint.str());
	ISlavePtr localMixer = this;
	adapter->add(localMixer, ic->stringToIdentity(remoteMixerName));
	adapter->activate();

	// start core
	mixerCore = new MixerCore(&(this->remoteHostsM));
	mixerCore->start();

	LOG4CXX_DEBUG(logger, "Mixer::Mixer adapter activated()");
	ic->waitForShutdown(); // TODO to remove
}

Mixer::~Mixer() {

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

bool Mixer::isConnected() const {
	return currentState != States::DISCONNECTED;
}

void Mixer::setLocalRtpPort(int port) {
	if ((port < 1024) || (port > 32768)) {
	} else {
		localRTPPort = port;
	}
}

int Mixer::getLocalRtpPort() const {
	return localRTPPort;
}

const IPV4Address* Mixer::getRemoteHost() const {
	return remoteAddr;
}

const IPV4Address* Mixer::getLocalHost() const {
	return localAddr;
}

void Mixer::changeState(int newState) {
	stringstream a;
	LOG4CXX_DEBUG(logger, "Mixer::changeState()");

	a << string("Mixer::changeState() prev: ") << this->currentState << " new: " << newState;
	LOG4CXX_DEBUG(logger, a.str());
	this->currentState = newState;
}

TerminalCapabilities Mixer::remoteGetCapabilities(const ::Ice::Current& curr) {
	// TODO
}

void Mixer::remoteTryConnect(const ::agh::CallParameters& params, const ::Ice::Identity& ident, const ::Ice::Current& curr) {
	stringstream a;
	LOG4CXX_DEBUG(logger, string("Mixer::remoteTryConnect()"));
	masterCallbackPrx = IMasterCallbackPrx::uncheckedCast(curr.con->createProxy(ident));

	if (localAddr) {
		delete localAddr;
	}
	if (remoteAddr) {
		delete remoteAddr;
	}

	IPV4Address *tmpAddr = new IPV4Address(getRemoteAddressFromConnection(curr.con));
	TerminalInfo *info = new TerminalInfo;
	info->address = *tmpAddr;
	info->rtpPort = params.masterRtpPort;
	info->outgoingCodec = params.outgoingCodec.id;
	info->incomingCodec = params.incomingCodec.id;
	info->transport = NULL;
	info->readedSize = 0;
	info->buf = NULL;
	remoteHostsM[tmpAddr->getHostname()] = info;
	a << "Mixer::remoteTryConnect() conf received, remote addr: " << tmpAddr << " port: " << params.masterRtpPort;
	LOG4CXX_DEBUG(logger, a.str());

	changeState(States::PASSIVE_CONNECTED);

	// inform remote site
	LOG4CXX_DEBUG(logger, string("Mixer::remoteTryConnect() sending ACK..."));
	CallParametersResponse response;
	response.slaveRtpPort = localRTPPort;

	// Response in new thread
	WorkerThread* tmpThread = new WorkerThread(this, response);
	tmpThread->start();
	LOG4CXX_DEBUG(logger, string("Mixer::remoteTryConnect() ACK has been sent"));
}

void Mixer::remoteStartTransmission(const ::Ice::Current& curr) {
	stringstream a;
	stringstream b;
	LOG4CXX_DEBUG(logger, string("Mixer::remoteStartTransmission()"));

	IPV4Address *tmpAddr = new IPV4Address(getRemoteAddressFromConnection(curr.con));
	a << string("Mixer::remoteStartTransmission() rem hostAddr: ") << tmpAddr->getHostname();
	LOG4CXX_DEBUG(logger, a.str());

	TerminalInfo *info = remoteHostsM[tmpAddr->getHostname()];
	if (remoteHostsM.find(tmpAddr->getHostname()) == remoteHostsM.end()  ) {
		cout << "ERROR info not found\n";
	}

	if (this->currentState != States::PASSIVE_CONNECTED) {
		LOG4CXX_DEBUG(logger, string("Mixer::remoteStartTransmission() bad state"));
	} else {
		changeState(States::PASSIVE_OPERATIONAL);

		cout << "TRANSCEIVER STARTED\n";

		// TODO start RTP.RTCP transmission
		CodecFactory codecfactory;
		Codec* codecInc = codecfactory.getCodec(AudioCodec::PCMU); // HACK
		// 		Codec* codecInc = codecfactory.getCodec(info->incomingCodec);
		Codec* codecOut = codecfactory.getCodec(AudioCodec::PCMU);
		//		Codec* codecOut = codecfactory.getCodec(info->outgoingCodec); // HACK

		info->transport = new TransportCCRTP();
		info->transport->setParams(codecInc->getFrameCount(), codecInc->getFrameSize());
		info->transport->setLocalEndpoint("0.0.0.0", localRTPPort);
		info->transport->setRemoteEndpoint(info->address, info->rtpPort);
		info->buf = new RingBuffer(1024*1024, 1);
		// 		b << "Mixer::remoteStartTransmission() creating transport,
		localRTPPort += 2;

		stringstream a;
		a << "rem address: " << info->address << " port: " << info->rtpPort;
		LOG4CXX_DEBUG(logger, a.str());

		info->transport->start();

		LOG4CXX_DEBUG(logger, string("Mixer::remoteStartTransmission() transmission started"));
	}
}

void Mixer::remoteDisengage(const ::Ice::Current& curr) {
	// TODO
}

int Mixer::remotePing(const Ice::Current& curr) {
	static int x = 0;
	return ++x;
}

void Mixer::foo(const ::Ice::Current& curr) {

}

} /* namespace agh */

int main(int argc, char **argv) {

	int lIcePort;
	int lRtpPort;
	int rDirPort;
	string rDirAddr;
	string alias;
	DirectoryPrx directory;

	if (argc < 6) {
		cout << "bad usage:\n\n";
		cout << "\t\tprogram lIcePort lRtpPort rDirPort rDirAddr alias\n\n";
		return 0;
	}

	/* arguments */
	lIcePort = atoi(argv[1]);
	lRtpPort = atoi(argv[2]);
	rDirPort = atoi(argv[3]);
	rDirAddr.append(argv[4]);
	alias.append(argv[5]);

	try {
		/* locate directory */
		Ice::CommunicatorPtr ic = Ice::initialize ();
		stringstream a;
		a << string("Directory") << ":default -h " << rDirAddr << " -p " << rDirPort;
		Ice::ObjectPrx base = ic->stringToProxy ( a.str() );
		directory = DirectoryPrx::checkedCast ( base );
		if ( !directory )
			throw "Invalid proxy";
		/* log in */
		TerminalAddress address;
		address.name = alias;
		std::ostringstream o2;
		o2 << string("") << lIcePort;
		address.port = o2.str();
		address.type = MCUTERMINAL;

		try {
			directory->registerTerminal(address);
		} catch(BadLoginException e) {
			cerr << "Bad login\n";
		} catch(TerminalExistsException e) {
			cerr << "Terminal already exists\n";
		}

	} catch(...) {
		clog << "Directory lookup error" << endl;
	}

	try {
		// Set up a simple configuration that logs on the console.
		BasicConfigurator::configure();
	} catch(log4cxx::helpers::Exception& e) {
		clog << "severe error" << endl;
	}

	Mixer m(lIcePort);
	m.setLocalRtpPort(lRtpPort);
}



