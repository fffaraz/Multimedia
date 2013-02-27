#include "../ice/icecommon.ice"

module agh {

	exception DirectoryException extends VoipException {};
	exception BadLoginException extends DirectoryException {};
	exception NoSuchTerminalException extends DirectoryException {};
	exception TerminalExistsException extends DirectoryException {};
	
	sequence<TerminalAddress> Terminals;
	
	interface Directory {
	
		void registerTerminal(TerminalAddress address)
			throws BadLoginException, TerminalExistsException;
		void removeTerminal(string name)
 			throws NoSuchTerminalException;
		TerminalAddress findTerminal(string name)
			throws NoSuchTerminalException;
			
		Terminals findTerminals(string name);
		Terminals getTerminals();
	};
};
