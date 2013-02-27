#ifndef AGHPHONEPREF_H
#define AGHPHONEPREF_H

#include <QDialog>
#include <string>
#include "configuration.h"
#include "ui_aghphonepref.h"
#include "terminal.h"
#include "master.h"
#include "icecommon.h"
#include "directory.h"
#include <iostream>
#include <iface.h>

using namespace agh;

class AghPhonePrefDialog : public QDialog, public Ui_Dialog {
	Q_OBJECT

	private:
		TerminalConfiguration *configuration;
		QList<int> incomingCodecs;
		QList<int> outgoingCodecs;
		
		void updateCodecLists();
	public:
		AghPhonePrefDialog(QWidget* parent = 0);
		
		void setConfiguration(TerminalConfiguration *configuration);
		vector<int> getIncomingCodecs();
		vector<int> getOutgoingCodecs();
		int getDirectoryPort();
		string getDirectoryAddress();
		string getUserAlias();
		int getPrefferedRTPPort();
		int getLocalPort();
		void updateConfiguration();
	
	private slots:
		void incomingUpButtonClicked();
		void incomingDownButtonClicked();
		void outgoingUpButtonClicked();
		void outgoingDownButtonClicked();
};

#endif

