#include "configuration.h"
#include "codecfactory.h"
#include "codec.h"

using namespace agh;
using namespace std;

namespace agh {

TerminalConfiguration::TerminalConfiguration() {
	incomingCodecs = new vector<int>();
	outgoingCodecs = new vector<int>();


	directoryPort = new string();
	directoryAddress = new string();
	directoryName = new string();
	directoryAlias = new string();
	prefferedRTPPort = new string();
	localPort = new string();
}

void TerminalConfiguration::setDefultValues() {
	/* directory conf */
	*directoryPort = "12345";
	*directoryAddress = "localhost";
	*directoryName = "Directory";
	*directoryAlias = "";

	/* local */
	*prefferedRTPPort = "14400";
	*localPort = "3000";

	/* mixer conf */

	/* codecs conf */
	CodecFactory codecfactory;
	*incomingCodecs = codecfactory.getSupportedCodecs();
	*outgoingCodecs = codecfactory.getSupportedCodecs();
}

string TerminalConfiguration::validateDirectory() {
	if (directoryAddress->size() <= 0) return "Directory address not specified!";
	if (directoryPort->size() <= 0) return "Directory port not specified!";

	return "";
}

} // namespace
