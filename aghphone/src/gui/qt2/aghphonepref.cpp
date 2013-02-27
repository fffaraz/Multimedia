#include <QtGui>
#include <stdio.h>
#include "aghphonepref.h"
#include "terminal.h"
#include "transceiver.h"
#include "transceiverfactory.h"
#include "codecfactory.h"
#include <iface.h>

using namespace std;
using namespace agh;

AghPhonePrefDialog::AghPhonePrefDialog(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	
	connect(incomingUpButton, SIGNAL(clicked()), this, SLOT(incomingUpButtonClicked()));
	connect(incomingDownButton, SIGNAL(clicked()), this, SLOT(incomingDownButtonClicked()));
	connect(outgoingUpButton, SIGNAL(clicked()), this, SLOT(outgoingUpButtonClicked()));
	connect(outgoingDownButton, SIGNAL(clicked()), this, SLOT(outgoingDownButtonClicked()));
	
	// validators
	QIntValidator *portValidator = new QIntValidator(1024, 65535, this);
	directoryPortEdit->setValidator(portValidator);
	localPortEdit->setValidator(portValidator);
	prefferedRTPPortEdit->setValidator(portValidator);
}

void AghPhonePrefDialog::updateCodecLists() {
	QStringList list;
	CodecFactory codecfactory;
	
	// incomingCodecsList
	incomingCodecsList->clear();
	for (int i = 0; i < incomingCodecs.size(); i++) {
		list << codecfactory.getCodecName(incomingCodecs.at(i)).c_str();
	}
	incomingCodecsList->addItems(list);
	list.clear();
	
	// CodecsList
	outgoingCodecsList->clear();
	for (int i = 0; i < outgoingCodecs.size(); i++) {
		list << codecfactory.getCodecName(outgoingCodecs.at(i)).c_str();
	}
	outgoingCodecsList->addItems(list);
}

void AghPhonePrefDialog::incomingUpButtonClicked() {
	int currentRow = incomingCodecsList->currentRow();
	QListWidgetItem* item = incomingCodecsList->item(currentRow);
	if ( incomingCodecsList->isItemSelected(item) ) {
		if (currentRow > 0) {
			incomingCodecs.swap(currentRow, currentRow-1);
			
			updateCodecLists();
			incomingCodecsList->setCurrentRow(currentRow-1);
		}
	}
}

void AghPhonePrefDialog::incomingDownButtonClicked() {
	int currentRow = incomingCodecsList->currentRow();
	QListWidgetItem* item = incomingCodecsList->item(currentRow);
	if ( incomingCodecsList->isItemSelected(item) ) {
		if (currentRow < incomingCodecsList->count()-1) {
			incomingCodecs.swap(currentRow, currentRow+1);
			
			updateCodecLists();
			incomingCodecsList->setCurrentRow(currentRow+1);
		}
	}
}

void AghPhonePrefDialog::outgoingUpButtonClicked() {
	int currentRow = outgoingCodecsList->currentRow();
	QListWidgetItem* item = outgoingCodecsList->item(currentRow);
	if ( outgoingCodecsList->isItemSelected(item) ) {
		if (currentRow > 0) {
			outgoingCodecs.swap(currentRow, currentRow-1);
			
			updateCodecLists();
			outgoingCodecsList->setCurrentRow(currentRow-1);
		}
	}
}

void AghPhonePrefDialog::outgoingDownButtonClicked() {
	int currentRow = outgoingCodecsList->currentRow();
	QListWidgetItem* item = outgoingCodecsList->item(currentRow);
	if ( outgoingCodecsList->isItemSelected(item) ) {
		if (currentRow < outgoingCodecsList->count()-1) {
			outgoingCodecs.swap(currentRow, currentRow+1);
			
			updateCodecLists();
			outgoingCodecsList->setCurrentRow(currentRow+1);
		}
	}
}

vector<int> AghPhonePrefDialog::getIncomingCodecs() {
	vector<int> list;
	
	for (int i = 0; i < incomingCodecs.size(); i++) {
		list.push_back(incomingCodecs[i]);
	}
	
	return list;
}

vector<int> AghPhonePrefDialog::getOutgoingCodecs() {
	vector<int> list;
	
	for (int i = 0; i < outgoingCodecs.size(); i++) {
		list.push_back(outgoingCodecs[i]);
	}
	
	return list;
}

int AghPhonePrefDialog::getDirectoryPort() {
	QString text = directoryPortEdit->text();
	
	if (text.size() <= 0) 
		return -1;
	else
		return text.toInt();
}
		
string AghPhonePrefDialog::getDirectoryAddress() {
	return directoryAddressEdit->text().toLocal8Bit().data();
}

string AghPhonePrefDialog::getUserAlias()  {
	return directoryAliasEdit->text().toLocal8Bit().data();
}
		
int AghPhonePrefDialog::getPrefferedRTPPort() {
	QString text = prefferedRTPPortEdit->text();
	
	if (text.size() <= 0) 
		return -1;
	else
		return text.toInt();
}
		
int AghPhonePrefDialog::getLocalPort() {
	QString text = localPortEdit->text();
	
	if (text.size() <= 0) 
		return -1;
	else
		return text.toInt();
}

void AghPhonePrefDialog::updateConfiguration() {
	*(configuration->directoryAddress) = directoryAddressEdit->text().toLocal8Bit().data();
	*(configuration->directoryPort) = directoryPortEdit->text().toLocal8Bit().data();
	*(configuration->directoryAlias) = directoryAliasEdit->text().toLocal8Bit().data();
	*(configuration->localPort) = localPortEdit->text().toLocal8Bit().data();
	*(configuration->prefferedRTPPort) = prefferedRTPPortEdit->text().toLocal8Bit().data();
	
	*(configuration->incomingCodecs) = this->getIncomingCodecs();
	*(configuration->outgoingCodecs) = this->getOutgoingCodecs();
}

void AghPhonePrefDialog::setConfiguration(TerminalConfiguration *configuration) {
	this->configuration = configuration;
	directoryAddressEdit->setText(configuration->directoryAddress->c_str());
	directoryPortEdit->setText(configuration->directoryPort->c_str());
	directoryAliasEdit->setText(configuration->directoryAlias->c_str());
	localPortEdit->setText(configuration->localPort->c_str());
	prefferedRTPPortEdit->setText(configuration->prefferedRTPPort->c_str());
	
	/* codecs */
	CodecFactory codecfactory;
	
	// find the first element of v that is even
	incomingCodecs.clear();
	for( int i = 0; i < configuration->incomingCodecs->size(); i++ ) {
		incomingCodecs.append(configuration->incomingCodecs->at(i));
	}
	outgoingCodecs.clear();
	for( int i = 0; i < configuration->outgoingCodecs->size(); i++ ) {
		outgoingCodecs.append(configuration->outgoingCodecs->at(i));
	}
	
	updateCodecLists();
}
