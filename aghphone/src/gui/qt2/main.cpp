#include <QApplication>
#include <QDialog>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>

// #include "aghphonepref.h"
#include "aghphonedialog.h"
#include "codecfactory.h"

int main(int argc, char *argv[]) {
	
	try {
		// Set up a simple configuration that logs on the console.
		log4cxx::BasicConfigurator::configure();
	} catch(log4cxx::helpers::Exception& e) {
		//clog << e.what() << endl;
	}
	
    QApplication app(argc, argv);
    AghPhoneDialog* dialog = new AghPhoneDialog();
    dialog->show();
    return app.exec();
}

