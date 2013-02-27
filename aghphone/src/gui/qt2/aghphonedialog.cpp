//#include <QtGui>
#include <stdio.h>
#include "aghphonedialog.h"
#include "aghphonepref.h"
#include <QDate>
#include "terminal.h"
#include "transceiver.h"
#include "transceiverfactory.h"
#include <iface.h>

using namespace std;
using namespace agh;

// TODO move threads to other files

/**
TODO:
Main window


najpierw tworzymy stala instancje terminala, ktora nasluchuje na podanych portach
[przydal by sie przycisk on-line], rejestruje callbacka, jezeli cos sie zglosi to
wyswietla monit, albo sie zgadzamy, jesli tak, zaczyna sie rozmowa, po tej rozmowie
chyba trzeba stworzyc nowy terminal,
do ustanawiania nowych rozmow mozna chyba uzyc bierzacego terminala? a ewentualnie po
rozmowie stworzyc nowa instancje.

- add about, license, and help dialogs [low]
- integrate with rest of application [high]
- icons [low]
- callbacks [medium]
- popup with incoming call [medium]
Preferences:
- conference (e.g. mixer) [medium]
 */

IMasterCallbackPrx masterCallback;


/* Threads  */
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
	try {
		masterCallback->remoteTryConnectAck(response);
	} catch(...) {
		cerr << "Exception[MainThread] unable to set up connection!\n";
		// TODO
	}
	cout << "sleep out\n";
}

class CallThread : public QThread {
private:
	Terminal *_terminal;
	string _hostAddress;
	int _remotePort;
public:
	CallThread(Terminal* terminal, string hostAddress, int remotePort);
	void run();
};

CallThread::CallThread(Terminal* terminal, string hostAddress, int remotePort) {
	_terminal = terminal;
	_hostAddress = hostAddress;
	_remotePort = remotePort;
}
void CallThread::run() {
	cout << "starting call thread\n";
	try {
		_terminal->connect(_hostAddress.c_str(), _remotePort);
	} catch(...) {
		cerr << "EXCEPTION[callThread]: unable to perform connection!\n";
		// TODO
	}
	cout << "finishing call thread\n";
}

/* Main application */


AghPhoneDialog::AghPhoneDialog(QWidget* parent) : QMainWindow(parent) {
	setupUi(this);

	connect(this, SIGNAL(stateChanged(int)), this, SLOT(stateTransitionSlot(int)));

	isRegistered = false;
	mainTerminal = 0;
	incCallThread = 0;
	mainThread = 0;

	configuration = new TerminalConfiguration();
	prefDialog = new AghPhonePrefDialog();
	searchDir = new AghPhoneSearchDirDialog();
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(":tray.png"));

	connect(callButton, SIGNAL(clicked()), this, SLOT(callButtonClicked()));
	connect(disengageButton, SIGNAL(clicked()), this, SLOT(disengageButtonClicked()));
	connect(contactsList, SIGNAL(itemSelectionChanged()), this, SLOT(contactSelected()));
	connect(hostAddressEdit, SIGNAL(textChanged(QString)), this, SLOT(contactTyped(QString)));
	connect(portEdit, SIGNAL(textChanged(QString)), this, SLOT(contactTyped(QString)));
	connect(menubar, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,  SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	// validators
	QIntValidator *portValidator = new QIntValidator(1024, 65535, this);
	portEdit->setValidator(portValidator);

	incCallDialog = new QMessageBox("Incoming connection", "Do you want to pick up the phone?",
			QMessageBox::Information, QMessageBox::Ok, QMessageBox::Cancel, 0, this);

	trayIcon->show();
	tempInit(); // temporary
	initializeTerminal();
}

/**
 * (Re)initailize terminal
 * sets configuration (transmitter, localIcePort)
 */
void AghPhoneDialog::initializeTerminal() {

	if (mainTerminal != 0) {
		// TODO is extra operation needed to reset Terminal state?
		mainTerminal->disengage();
	} else {
		mainTerminal = new Terminal(atoi(configuration->localPort->c_str()));
		// register callback
		mainTerminal->registerCallback(this);
	}

	// set configuration
	mainTerminal->setLocalIcePort(atoi( configuration->localPort->c_str() ));
	TransceiverFactory factory;
	Transceiver* transceiver = factory.getTransceiver("alsa", "ccrtp");
	mainTerminal->setTransceiver(transceiver);
}

void AghPhoneDialog::trayActivated(QSystemTrayIcon::ActivationReason reason) {

	if (this->isEnabled()) {
		this->showMinimized();
	}
	else
		this->showNormal();
	this->setHidden(this->isEnabled());
	this->setEnabled(!this->isEnabled());
}

void AghPhoneDialog::tempInit() {

	// configuration class - initalization & propagation to the preference dialog
//	configuration->setDefultValues();
	configuration->loadConf();
	prefDialog->setConfiguration(configuration);
	searchDir->setConfiguration(configuration);

	// icons
	this->callButton->setIcon(QIcon(":call.png"));
	this->disengageButton->setIcon(QIcon(":disengage.png"));
	this->callButton->setText("");
	this->disengageButton->setText("");
}

void AghPhoneDialog::putTerminalIntoList(TerminalAddress *terminal, QString alias) {
	if (terminal == NULL) {
		return;
	}
	// check whether exists
	QList<QListWidgetItem *> resultList = contactsList->findItems(alias, Qt::MatchExactly);
	if ( resultList.count() > 0)
		return;

	QListWidgetItem *tmpItem = new QListWidgetItem(alias);
	tmpItem->setData(Qt::UserRole, QVariant(QMetaType::VoidStar, &terminal) );

	contactsList->addItem ( tmpItem );
}

void AghPhoneDialog::contactTyped(const QString& string) {

	if ( hostAddressEdit->text().length() <= 0 || portEdit->text().length() <= 0) {
		callButton->setEnabled(false);
		contactsList->clearSelection();
	}
	else {
		callButton->setEnabled(true);
	}
}

void AghPhoneDialog::contactSelected() {

	int currentRow = contactsList->currentRow();

	QListWidgetItem* item = contactsList->item(currentRow);
	if ( contactsList->isItemSelected(item) ) {
		QString text;
		if (item != NULL) {
			text = item->text();
		} else {
			text = "";
		}
		// get Terminal
		TerminalAddress *tmpTerminal = reinterpret_cast<TerminalAddress*>(qvariant_cast<void*>(item->data(Qt::UserRole)));
		hostAddressEdit->setText(tmpTerminal->name.c_str());
		portEdit->setText(tmpTerminal->port.c_str());
	}
}

void AghPhoneDialog::callButtonClicked() {
	cout << "Starting call...\n";

	callButton->setEnabled(false);
	disengageButton->setEnabled(true);

	QString message("Calling ");
	message += hostAddressEdit->text();
	message += " on port " + portEdit->text();
	message += " ...";
	statusbar->showMessage(message);

	// set terminal parameters & add to list
	QString tmpAlias(hostAddressEdit->text());
	tmpAlias += ":";
	tmpAlias += portEdit->text();
	TerminalAddress *tmpTerminalAddr = new TerminalAddress;
	tmpTerminalAddr->ipAddress = hostAddressEdit->text().toLocal8Bit().data();
	tmpTerminalAddr->port = portEdit->text().toLocal8Bit().data();
	tmpTerminalAddr->name = hostAddressEdit->text().toLocal8Bit().data();
	putTerminalIntoList(tmpTerminalAddr, tmpAlias);

	// call procedure
	mainTerminal->setLocalIcePort(atoi(this->configuration->localPort->c_str()));
	//	mainTerminal->setLocalRtpPort(atoi(this->configuration->prefferedRTPPort->c_str()));
	mainTerminal->setLocalRtpPort(2002);

	string hostAddress = hostAddressEdit->text().toLocal8Bit().data();
	string remotePort =  portEdit->text().toLocal8Bit().data();
	cout << "Trying to connect...\n";
	cout << "HAddr: " + hostAddress << " RPort: " << remotePort
	<< " LIcePort: " << mainTerminal->getLocalIcePort() << endl;

	// start connection in new thread
	mainThread = new CallThread(mainTerminal, hostAddress, atoi(remotePort.c_str()));
	mainThread->start();
}

void AghPhoneDialog::disengageButtonClicked() {

	disengageButton->setEnabled(false);
	callButton->setEnabled(true);

	QString message("Calling ");
	message += hostAddressEdit->text();
	message += " stopped.";
	statusbar->showMessage(message, 5000);

	mainTerminal->disengage();
	// reinitialize
	if (mainThread != 0) {
		mainThread->terminate();
	}
	if (incCallThread != 0) {
		incCallThread->terminate();
	}
	this->initializeTerminal();

	incCallThread = 0;
	mainThread = 0;
}

void AghPhoneDialog::menuTriggered(QAction *action) {
	cout << action->text().toLocal8Bit().data() << endl;
	QString operation = action->text();

	if (operation == "Preferences") {
		prefDialog->exec();
		prefDialog->updateConfiguration();
		configuration->saveConf();
	} else if (operation == "Close") {
		terminateApplication();
	} else if (operation == "Search in directory") {
		searchDir->exec();
		TerminalAddress *foundTerminal = searchDir->getSelectedTerminal();

		// check whether terminal was selected
		if (foundTerminal == NULL) {
			return;
		}

		QString foundTermName = foundTerminal->name.c_str();
		foundTermName += ":";
		foundTermName += foundTerminal->port.c_str();
		if (foundTerminal != 0) {
			this->putTerminalIntoList(foundTerminal, foundTermName);
		}
	} else if (operation == "Log in") {
		registeredAddress = new TerminalAddress;

		if (isRegistered) {
			QMessageBox::critical( this, this->windowTitle(),
					"User already registered");
			return;
		}

		if (this->configuration->directoryAlias->size() <= 0) {
			QMessageBox::critical( this, this->windowTitle(),
					"User alias not specified! Check your configuration.");
			return;
		}

		if (this->configuration->localPort->size() <= 0) {
			QMessageBox::critical( this, this->windowTitle(),
					"Local port not specified! Check your configuration.");
			return;
		}

		registeredAddress->name = *(this->configuration->directoryAlias);
		registeredAddress->port = *(this->configuration->localPort);

		// Get directory reference
		QString errorMessage = this->configuration->validateDirectory().c_str();

		if (errorMessage.count() > 0) {
			QMessageBox::critical( this, this->windowTitle(),
					errorMessage);

		} else {
			// search for directory
			try {
				Ice::CommunicatorPtr ic = Ice::initialize();
				stringstream a;
				a << *(configuration->directoryName)
				<< ":default -h " << *(configuration->directoryAddress)
				<< " -p " << *(configuration->directoryPort);
				Ice::ObjectPrx base = ic->stringToProxy ( a.str() );
				DirectoryPrx directory = DirectoryPrx::checkedCast ( base );

				if ( !directory ) {
					QMessageBox::critical( this, this->windowTitle(),
							"Connection to directory failed, please check your configuration!");
				} else {
					directory->registerTerminal(*registeredAddress);
					isRegistered = true;


					QString message("Terminal ");
					message += registeredAddress->name.c_str();
					message += " successfully registered.";
					statusbar->showMessage(message, 5000);
				}

				ic->destroy();

			} catch (TerminalExistsException ex) {
				QMessageBox::critical( this, this->windowTitle(),
						"User already registered, change your alias!");
				return;
			}  catch (...) {
				QMessageBox::critical( this, this->windowTitle(),
						"Connection to directory failed, please check your configuration!");
				return;
			}
		}

	} else if (operation == "Log out") {

		if (!isRegistered) {
			QMessageBox::critical( this, this->windowTitle(),
					"User not registered");
			return;
		}

		// Get directory reference
		QString errorMessage = this->configuration->validateDirectory().c_str();

		if (errorMessage.count() > 0) {
			QMessageBox::critical( this, this->windowTitle(),
					errorMessage);

		} else {
			// search for directory
			try {
				Ice::CommunicatorPtr ic = Ice::initialize();
				stringstream a;
				a << *(configuration->directoryName)
				<< ":default -h " << *(configuration->directoryAddress)
				<< " -p " << *(configuration->directoryPort);
				Ice::ObjectPrx base = ic->stringToProxy ( a.str() );
				DirectoryPrx directory = DirectoryPrx::checkedCast ( base );

				if ( !directory ) {
					QMessageBox::critical( this, this->windowTitle(),
							"Connection to directory failed, please check your configuration!");
				} else {
					directory->removeTerminal(registeredAddress->name);

					QString message("Terminal ");
					message += registeredAddress->name.c_str();
					message += " successfully unregistered.";
					statusbar->showMessage(message, 5000);

					delete registeredAddress;
					isRegistered = false;
				}

				ic->destroy();

			} catch (NoSuchTerminalException ex) {
				QMessageBox::critical( this, this->windowTitle(),
						"Such user does not exists in directory!");
			}  catch (...) {
				QMessageBox::critical( this, this->windowTitle(),
						"Connection to directory failed, please check your configuration!");
				return;
			}
		}
	} else if (operation == "About") {
		QString about;
		QDate curDate = QDate::currentDate();
		about += "AGHPhone VoIP application\n";
		about += "\n\nVersion 0.9.0.2b from ";
		about += curDate.toString();
		about += "\n\nCopyright (C) 2008  Mateusz Kramarczyk <kramarczyk@gmail.com>\n";
		about += "Copyright (C) 2008  Tomasz Kijas <kijasek@gmail.com>\n";
		about += "Copyright (C) 2008  Tomir Kryza <tkryza@gmail.com>\n";
		about += "Copyright (C) 2008  Maciej Kluczny <kluczny@fr.pl>\n";
		about += "Copyright (C) 2008  AGH University of Science and Technology <www.agh.edu.pl>\n";
		QMessageBox::about(this, "About AGHPhone", about);
	} else if (operation == "About Qt") {
		QMessageBox::aboutQt(this, "About Qt");
	} else if (operation == "License") {
		QString msg;
		msg += "This program is free software: you can redistribute it and/or modify\n";
		msg += "it under the terms of the GNU General Public License as published by\n";
		msg += "the Free Software Foundation, either version 3 of the License, or\n";
		msg += "(at your option) any later version.\n";
		msg += "\n";
		msg += "This program is distributed in the hope that it will be useful,\n";
		msg += "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
		msg += "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
		msg += "GNU General Public License for more details.\n";
		msg += "\n";
		msg += "You should have received a copy of the GNU General Public License\n";
		msg += "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";

		QMessageBox::about(this, "AGHPhone license regulations", msg);
	}
}

void AghPhoneDialog::stateTransitionSlot(int curState) {
	string message;
	if (curState == States::PASSIVE_CONNECTED) {

		int ret = incCallDialog->exec();
		if (ret == QMessageBox::Ok) {
			cout << "Ok button clicked\n";
			masterCallback = mainTerminal->getMasterCallback();
			QtWorkerThread *p = new QtWorkerThread(mainTerminal);
			p->start();
		} else if (ret == QMessageBox::Cancel) {
			cout << "Cancel button clicked\n";
			masterCallback->remoteTryConnectNack();
		}
	}
	else if (curState == States::ACTIVE_CONNECTED) {
		message = "Connected, requesting parameters";
		cout << message << endl;
		statusbar->showMessage(message.c_str());
	}
	else if (curState == States::ACTIVE_OPERATIONAL) {
		message = "Transmission started...";
		cout << message << endl;
		statusbar->clearMessage();
		statusbar->showMessage(message.c_str(), 5000);
	}
	else if (curState == States::DISCONNECTED) {
		// TODO reset environment
		message = "Disconnected";
		cout << message << endl;
		statusbar->showMessage(message.c_str());
	}
}

bool AghPhoneDialog::onStateTransition(int prevState, int curState, const IPV4Address& addr) {
	cout << "state transition\n";
	cout << "\t from: " << prevState << " to: " << curState << endl;

	string stateName;
	switch (curState) {
	case	States::ACTIVE_CONNECTED: stateName.append("ACTIVE_CONNECTED"); break;
	case	States::ACTIVE_OPERATIONAL: stateName.append("ACTIVE_OPERATIONAL"); break;
	case	States::DISCONNECTED: stateName.append("DISCONNECTED"); break;
	case	States::PASSIVE_CONNECTED: stateName.append("PASSIVE_CONNECTED"); break;
	case	States::PASSIVE_OPERATIONAL: stateName.append("PASSIVE_OPERATIONAL"); break;
	default: stateName.append("unknown state");
	}

	cout << "Current state name: " << stateName << endl;
	emit stateChanged(curState);
}

void AghPhoneDialog::terminateApplication() {
	exit(1);
}

