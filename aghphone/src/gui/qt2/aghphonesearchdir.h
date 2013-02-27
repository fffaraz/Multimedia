#ifndef AGHPHONESEARCHDIR_H
#define AGHPHONESEARCHDIR_H

#include <QDialog>
#include <string>
#include "ui_aghphonesearchdir.h"
#include "icecommon.h"
#include "directory.h"
#include "configuration.h"
#include <iface.h>
#include <IceUtil/Thread.h>
#include "terminal.h"
#include "master.h"

using namespace agh;

class AghPhoneSearchDirDialog : public QDialog, public Ui_SearchDialog {
	Q_OBJECT

	private:
		DirectoryPrx directory;
		TerminalConfiguration *configuration;
		TerminalAddress* selectedTerminal;

	public:
		AghPhoneSearchDirDialog(QWidget* parent = 0);
		void setConfiguration(TerminalConfiguration *configuration);
		virtual void exec();
		void putTerminalIntoList(TerminalAddress *terminal, QString alias);
		void putTerminalIntoTable(TerminalAddress *terminal, QString alias);
		TerminalAddress* getSelectedTerminal();

	private slots:
		void searchButtonClicked();
};

#endif


