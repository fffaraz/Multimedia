/*
 * Copyright (C) 2008  Mateusz Kramarczyk <kramarczyk (at) gmail (dot) com>
 * Copyright (C) 2008  Tomasz Kijas <kijasek (at) gmail (dot) com>
 * Copyright (C) 2008  Tomir Kryza <tkryza (at) gmail (dot) com>
 * Copyright (C) 2008  Maciej Kluczny <kluczny (at) fr (dot) pl>
 * Copyright (C) 2008  AGH University of Science and Technology <www.agh.edu.pl>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "audio.h"
#include "transport.h"
#include "audioalsa.h"
#include "audiopa.h"
#include "transportccrtp.h"
#include "transceiverfactory.h"
#include <string>

using namespace std;

namespace agh {

TransceiverFactory::TransceiverFactory()
{
}

TransceiverFactory::~TransceiverFactory()
{
}

Transceiver* TransceiverFactory::getTransceiver(const string& audio, const string& transport)
{
	Transceiver* transceiver = new Transceiver();
	Audio* a = NULL;
	Transport* t = NULL;

	cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd1\n";
	if(audio == "alsa") {
		a = new AudioAlsa(transceiver);
		cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd2\n";
	} else if(audio == "pa") {
		a = new AudioPa(transceiver);
		cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd3\n";
	} else {
		return NULL;
	}
	cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd4\n";

	if(transport == "ccrtp") {
		t = new TransportCCRTP();
	} else {
		delete a;
		return NULL;
	}
	cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd5\n";

	transceiver->setAudio(a);
	cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd6\n";
	transceiver->setTransport(t);
	cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDdd7\n";

	return transceiver;
}

}
