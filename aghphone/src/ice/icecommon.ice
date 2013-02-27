module agh {

  /**
   *  General Exception thrown by remote terminal
   */
	exception VoipException {
	};
	
	/**
	 *  USERTERMINAL - user terminal,
	 *  MCUTERMINAL - mixer terminal
	 */
	enum TerminalType {
		USERTERMINAL, MCUTERMINAL
	};
	
	/**
	  * Thrown when terminal participates another conversation
	  */
	exception TerminalBusyException extends VoipException {
	};
	
	/**
	 *  Thrown when user rejects a new incoming connection request
	 */
	exception ConnectionRejectedException extends VoipException {
	};

	/**
	 * id - codec number according to RFC3551
	 */
	struct ICodec {
		int id;
	};

	sequence<ICodec> Codecs;

	/**
	 * used for set negotiated call parameters for local site
	 * outgoingCodec - codec used for outgoing connection (local -> remote)
	 * incomingCodec - codec used for incoming connection (local <- remote)
	 * masterRtpPort - local RTP/RTCP port
	 */
	struct CallParameters {
		ICodec outgoingCodec;
		ICodec incomingCodec;
		int masterRtpPort;
	};

	/**
	 * used for set negotiated call parameters for remote site
	 * slaveRtpPort - remote RTP/RTCP port
	 */
	struct CallParametersResponse {
		int slaveRtpPort;
	};

	/**
	 * used for negotiating codecs during connection between local and remote terminals
	 * preferredOutgoingCodecs - list of preferred outgoing codecs
	 * preferredIncomingCodecs - list of preferred incoming codecs
	 */
	struct TerminalCapabilities {
		Codecs preferredOutgoingCodecs;
		Codecs preferredIncomingCodecs;
	};

	/**
	 * used by terminals in order to register in directory
	 * ipAddress - inet address terminal registers under
	 * port - port terminal registers under
	 * name - name of terminal it registers under
	 * type - type of terminal (see @TerminalType)
	 */
	struct TerminalAddress {
		string ipAddress;
		string port;
		string name;
		TerminalType type;
	};
};
