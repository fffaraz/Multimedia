#include <QApplication>
#include <QDialog>
#include <vector>
#include "aghphonepref2.h"
#include "codecfactory.h"

using namespace std;

int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     AghPhoneDialog* dialog = new AghPhoneDialog;
//     dialog->show();
//     return app.exec();
	
	CodecFactory codecfactory;
	vector<int> codecs = codecfactory.getSupportedCodecs(); 
	
	QApplication app(argc, argv);
	AghPhonePrefDialog* dialog = new AghPhonePrefDialog;
	dialog->setSupportedCodecs(codecs);
	dialog->show();
	return app.exec();
}

