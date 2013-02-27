#ifndef AGHPHONECONFIGURATION_H
#define AGHPHONECONFIGURATION_H

#include <string>
#include <vector>

namespace agh {

using namespace std;
// using namespace ost;

class TerminalConfiguration {
	
	private:
	
	public:
		/* directory conf */
		string *directoryPort;
		string *directoryAddress;
		string *directoryName;
		string *directoryAlias;
		
		/* local */
		string *prefferedRTPPort;
		string *localPort;
		
		/* mixer conf */
		
		/* codecs conf */
		vector<int> *incomingCodecs;
		vector<int> *outgoingCodecs;
	
		TerminalConfiguration();
		
		void setDefultValues();
		string validateDirectory();	// "" (empty string) means ok.
};

} // namespace
		
#endif
