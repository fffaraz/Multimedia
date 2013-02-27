#ifndef AGHPHONEDIALOG_H
#define AGHPHONEDIALOG_H

#include <qthread.h>
#include <QDialog>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "aghphonepref.h"
#include "aghphonesearchdir.h"
#include "ui_aghphonedialog.h"
#include "terminal.h"
#include "master.h"
#include "icecommon.h"
#include "directory.h"
#include "configuration.h"
#include "qtguicommon.h"
#include <iostream>
#include <iface.h>
#include <QSound>

using namespace agh;


// class AghPhonePrefDialog;

class AghPhoneDialog : public QMainWindow, public Ui_MainWindow, public IUICallback {
	Q_OBJECT

	private:
		bool isRegistered;
		TerminalAddress *registeredAddress;
		TerminalConfiguration *configuration;
		QSystemTrayIcon *trayIcon;
		Terminal *mainTerminal;
		QSound *ringSound;

		QMessageBox *incCallDialog;
		QThread *incCallThread;
		QThread *mainThread;

		void tempInit();
		AghPhonePrefDialog *prefDialog;
		AghPhoneSearchDirDialog *searchDir;
		void initializeTerminal(); 		// initialize listening terminal (when we're going online)
		void terminateApplication();
		void putTerminalIntoList(TerminalAddress *terminal, QString alias);

		virtual bool onStateTransition(int prevState, int curState, const IPV4Address& addr);

	public:
		AghPhoneDialog(QWidget* parent = 0);

	private slots:
		void callButtonClicked();
		void disengageButtonClicked();
		void contactSelected();
		void contactTyped(const QString& string);
		void menuTriggered(QAction *action);
		void trayActivated(QSystemTrayIcon::ActivationReason reason);
		void stateTransitionSlot(int curState);

	signals:
		void stateChanged(int curState);
};

#endif

