#include <Ice/Ice.h>
#include <map>
#include <cc++/address.h>
#include "../ice/icecommon.h"
#include "directory.h"
#include "tools.h"

using namespace std;
using namespace agh;
using namespace ost;

class DirectoryImpl : public Directory
{
	private:
		map<string, TerminalAddress> terminals;
	public:
		virtual void registerTerminal(const TerminalAddress& address, const Ice::Current&);
		virtual void removeTerminal(const string& name, const Ice::Current&);
		virtual TerminalAddress findTerminal(const string& name, const Ice::Current&);
		virtual Terminals getTerminals(const Ice::Current&);
		virtual Terminals findTerminals(const string& name, const Ice::Current&);
};

Terminals DirectoryImpl::findTerminals(const string& name, const Ice::Current&) {
	map<string, TerminalAddress>::iterator iter;
	Terminals retTerminals;

	for( iter = terminals.begin(); iter != terminals.end(); iter++ ) {
		if (iter->first.find(name) != string::npos)
			retTerminals.push_back(iter->second);
	}

	return retTerminals;
}

Terminals DirectoryImpl::getTerminals(const Ice::Current&) {
	map<string, TerminalAddress>::iterator iter;
	Terminals retTerminals;

	for( iter = terminals.begin(); iter != terminals.end(); iter++ ) {
		retTerminals.push_back(iter->second);
	}

	return retTerminals;
}

void DirectoryImpl::registerTerminal(const TerminalAddress& address, const Ice::Current& curr) {

	IPV4Address *remoteAddr = new IPV4Address(getRemoteAddressFromConnection(curr.con));

	TerminalAddress newAddress;
	newAddress.name = address.name;
	newAddress.ipAddress = remoteAddr->getHostname();
	newAddress.port = address.port;
	newAddress.type = address.type;

	if (newAddress.name == "") {
		throw BadLoginException();
	}
	if (terminals.count(newAddress.name) > 0) {
		throw TerminalExistsException();
	}

	terminals[newAddress.name] = newAddress;

	cout << newAddress.name << " successfully registered [" << newAddress.ipAddress << ":" << newAddress.port << "]" << endl;
}

void DirectoryImpl::removeTerminal(const string& name, const Ice::Current&) {

	if (terminals.count(name) <= 0) {
		throw NoSuchTerminalException();
	}

	terminals.erase(name);

	cout << name << " successfully unregistered" << endl;
}

TerminalAddress DirectoryImpl::findTerminal(const string& name, const Ice::Current&) {

	if (terminals.count(name) <= 0) {
		throw NoSuchTerminalException();
	}

	TerminalAddress retAddress = terminals[name];

	return retAddress;
}

int main ( int argc, char* argv[] )
{
	if (argc < 3) {
		cout << "bad parameters\n";
		cout << "usage: directory name port\n";
		return 1;
	}

	string name = argv[1];
	string port = argv[2];

	int status = 0;
	Ice::CommunicatorPtr ic;
	try
	{
		ic = Ice::initialize ( );
		stringstream iceEndpoint;
		iceEndpoint << "default -p " << port;

		Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints(name,
				iceEndpoint.str());

		Ice::ObjectPtr object = new DirectoryImpl;
		adapter->add( object, ic->stringToIdentity(name));
		adapter->activate();

		ic->waitForShutdown();

	}
	catch ( const Ice::Exception& e )
	{
		cerr << e << endl;
		status = 1;
	}
	catch ( const char* msg )
	{
		cerr << msg << endl;
		status = 1;
	}
	if ( ic )
	{
		try
		{
			ic->destroy();
		}
		catch ( const Ice::Exception& e )
		{
			cerr << e << endl;
			status = 1;
		}
	}

	return status;
}
