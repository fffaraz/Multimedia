#include <QtGui>
#include <stdio.h>
#include "aghphonedialog.h"
#include "terminal.h"
#include "transceiver.h"
#include "transceiverfactory.h"
#include <iface.h>

using namespace std;
using namespace agh;

IMasterCallbackPrx masterCallback;

class QtWorkerThread : public QThread {
	private:
		Terminal *terminal;
	public:
		QtWorkerThread(Terminal* terminal);
		void run();
};

QtWorkerThread::QtWorkerThread(Terminal* terminal) {
	this->terminal = terminal;
}

void QtWorkerThread::run() {
	cout << "sleep in\n";
	cout << "Ringin... Ringing ... phi phi....\n";
	cout << "enter local RTP port(must be odd number): ";
	int localRTPPort = 5008; // HACK hardcoded
	//Thread::sleep(2000);
	this->terminal->setLocalRtpPort(localRTPPort);
	CallParametersResponse response;
	response.slaveRtpPort = localRTPPort;
	masterCallback->remoteTryConnectAck(response);
	cout << "sleep out\n";
}

AghPhoneDialog::AghPhoneDialog(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	connect(callButton, SIGNAL(clicked()), this, SLOT(callSlave()));
}

void AghPhoneDialog::callSlave() {
	cout << "klikles cos tam" << endl;
	QString s = ip_part_1->text();
	QByteArray ba = s.toLatin1();
	const char *c = ba.data();
	printf("%s\n", c);

	this->lIcePort = local_port->text().toInt();
	this->rIcePort = remote_port->text().toInt();

	char address[80];

	sprintf(address, "%s.%s.%s.%s",
		ip_part_1->text().toLatin1().data(), ip_part_2->text().toLatin1().data(),
		ip_part_3->text().toLatin1().data(), ip_part_4->text().toLatin1().data());
	printf("address: %s:%d\n", address, rIcePort);

	terminal = new Terminal(lIcePort);
	terminal->registerCallback(this);

	TransceiverFactory factory;
	Transceiver* transceiver = factory.getTransceiver("alsa", "ccrtp");

	terminal->setTransceiver(transceiver);
	terminal->setLocalRtpPort(this->lIcePort);
	terminal->connect(address, this->rIcePort);
}

bool AghPhoneDialog::onStateTransition(int prevState, int curState, const IPV4Address& addr) {
	if (curState == States::PASSIVE_CONNECTED) {
		masterCallback = terminal->getMasterCallback();
		QtWorkerThread *p = new QtWorkerThread(terminal);
		p->start();
	}
}
