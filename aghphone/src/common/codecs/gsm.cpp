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
extern "C" {
#include <gsm/gsm.h>
}
#include "gsm.h"

namespace agh {

GSM::GSM() {
	r = ::gsm_create();
}

GSM::~GSM() {
	::gsm_destroy(r);
}

int GSM::getDelay() {
	return 20;
}

int GSM::getFrameCount() {
	return 160;
}

int GSM::getFrameSize() {
	return 2;
}

float GSM::getFrequency() {
	return 8000.0f;
}

int GSM::encode(char *dest, char *src) {
	::gsm_encode(r, (::gsm_signal *) src, (::gsm_byte *) dest);
	return 33;
}

int GSM::decode(char *dest, char *src, int srcsize) {
	::gsm_decode(r, (::gsm_byte *) src, (::gsm_signal *) dest);
	return 320;
}

}