#include <Ice/Identity.ice>
#include "icecommon.ice"

module agh {

	/**
	 * The callback local terminal registers in remote terminal
	 * remoteTryConnectAck -> notifies local terminal when user accept incoming call
	 * remoteTryConnectNack -> opposite to remoteTryConnectAck
	 */
	interface IMasterCallback {
	
		void remoteTryConnectAck(CallParametersResponse response);	
		void remoteTryConnectNack();
	};
	
	/**
	 * a part of remote terminal interface
	 * remoteGetCapabilities -> gets remote terminal's capabilities
	 * remoteTryConnect -> tries connect to remote terminal
	 * remoteStartTransmission -> starts remote transceiver
	 * remoteDisengage -> disconnects remote terminal
	 */
	interface ISlave {
		
		TerminalCapabilities remoteGetCapabilities();
		
		void remoteTryConnect(CallParameters parameters, ::Ice::Identity masterIdent)
								throws TerminalBusyException, ConnectionRejectedException;
		
		void remoteStartTransmission();
		
		void remoteDisengage();
	
	};

};
