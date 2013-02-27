#include <QtGui>
#include <stdio.h>
#include "aghphonesearchdir.h"

using namespace std;
using namespace agh;

AghPhoneSearchDirDialog::AghPhoneSearchDirDialog(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	connect(searchButton, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
	selectedTerminal = 0;
	resultTAble->setColumnCount(1);
	resultTAble->setRowCount(0);
	QStringList stringList;
	stringList.append("Terminal alias");
	resultTAble->setHorizontalHeaderLabels(stringList);
}

void AghPhoneSearchDirDialog::setConfiguration(TerminalConfiguration *configuration) {
	this->configuration = configuration;
}

void AghPhoneSearchDirDialog::searchButtonClicked() {
	QString criteria = aliasEdit->text();
	TerminalAddress* terminalAddress;

	try {
		Terminals terminals = this->directory->findTerminals(criteria.toLocal8Bit().data());

		// add founded address to the list
		for (int i = 0; i < terminals.size(); i++) {
			terminalAddress = new TerminalAddress;
			cout << "termianal port: " << terminalAddress->port << endl;
			*terminalAddress = terminals[i];
			putTerminalIntoTable(terminalAddress, terminalAddress->name.c_str());
		}

	} catch(VoipException) {
		QMessageBox::critical( this, this->windowTitle(),
				"Unable to retrieve terminal address!");
		return;
	}
}

void AghPhoneSearchDirDialog::putTerminalIntoTable(TerminalAddress *terminal, QString alias) {
	if (terminal == NULL) {
		return;
	}
	// check whether exists
	QList<QTableWidgetItem *> tmpList = resultTAble->findItems(alias, Qt::MatchExactly);
	if ( tmpList.count() > 0)
		return;

	QTableWidgetItem *tmpItem = new QTableWidgetItem(alias);
	if (terminal->type == USERTERMINAL) {
		tmpItem->setIcon(QIcon(":contactuser.png"));
	} else if (terminal->type == MCUTERMINAL) {
		tmpItem->setIcon(QIcon(":contactmcu.png"));
	} else {
		tmpItem->setIcon(QIcon(":tray.png"));
	}

	tmpItem->setFlags(Qt::ItemIsEnabled);
	tmpItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tmpItem->setData(Qt::UserRole, QVariant(QMetaType::VoidStar, &terminal) );
	int rowCout = resultTAble->rowCount();
	resultTAble->insertRow(rowCout);
	resultTAble->setItem(rowCout, 0, tmpItem);
}

void AghPhoneSearchDirDialog::putTerminalIntoList(TerminalAddress *terminal, QString alias) {

}

TerminalAddress* AghPhoneSearchDirDialog::getSelectedTerminal() {
	return selectedTerminal;
}

void AghPhoneSearchDirDialog::exec() {
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
			cout << a.str() << endl;

			Ice::ObjectPrx base = ic->stringToProxy ( a.str() );
			directory = DirectoryPrx::checkedCast ( base );

			if ( !directory ) {
				QMessageBox::critical( this, this->windowTitle(),
						"Connection to directory failed, please check your configuration!");
			} else {
				QDialog::exec();

				int currentRow = resultTAble->currentRow();
				cout << "currentRow: " << currentRow << endl;
				QTableWidgetItem* item = resultTAble->item(currentRow, 0);

				if (item != NULL) {
					// get Terminal
					selectedTerminal = reinterpret_cast<TerminalAddress*>(qvariant_cast<void*>(item->data(Qt::UserRole)));
					cout << "XXX1: " << selectedTerminal->name.c_str() << endl;
				} else {
					selectedTerminal = 0;
				}
			}

			ic->destroy();

		} catch (...) {
			QMessageBox::critical( this, this->windowTitle(),
					"Connection to directory failed, please check your configuration!");
			return;
		}
	}
}
