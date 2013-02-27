#ifndef AGHPHONEDIALOG_H
#define AGHPHONEDIALOG_H

#include <QDialog>
#include "ui_aghphonedialog.h"
#include "terminal.h"
#include "master.h"
#include "icecommon.h"
#include "directory.h"
#include <iostream>
#include <iface.h>

using namespace agh;

class AghPhoneDialog : public QDialog, public Ui::AghPhoneUI, public IUICallback {
	Q_OBJECT

	private:
		int lIcePort, rIcePort;
		Terminal *terminal;

	public:
	AghPhoneDialog(QWidget* parent = 0);

	virtual bool onStateTransition(int prevState, int curState, const IPV4Address& addr);	

	private slots:
	void callSlave();
};

#endif

