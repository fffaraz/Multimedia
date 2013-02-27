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

#include "ilbc20.h"

namespace agh {
	
ILBC20::ILBC20() {
    ::initEncode(&iLBCenc_inst, 20);
    ::initDecode(&iLBCdec_inst, 20, 1);
}

ILBC20::~ILBC20() {
	
}

int ILBC20::getDelay() {
	return 20;
} 

int ILBC20::getFrameCount() {
	return 160;
}

int ILBC20::getFrameSize() {
	return 2;
}

float ILBC20::getFrequency() {
	return 8000.0f;
}

int ILBC20::encode(char *dest, char *src) {
	for (int i = 0; i < getFrameCount(); ++i) {
		buf[i] = (float) ((short *)src)[i];
	}
	::iLBC_encode((unsigned char *)dest, buf, &iLBCenc_inst);
	return iLBCenc_inst.no_of_bytes;
}

int ILBC20::decode(char *dest, char *src, int srcsize) {
	::iLBC_decode(buf, (unsigned char *)src, &iLBCdec_inst, 1);
	for (int i = 0; i < getFrameCount(); ++i) {
		float t = buf[i];
		if (t < MIN_SAMPLE) {
			t = MIN_SAMPLE;
		}
		if (t > MAX_SAMPLE) {
			t = MAX_SAMPLE;
		}
		((short *) dest)[i] = (short) t;
	}
	return 320;
}

}