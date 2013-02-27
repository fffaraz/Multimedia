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

#include "g711.h"

extern "C" {
#include "legacy/g711/g711.h"
}

namespace agh {

G711u::G711u() {
}

G711u::~G711u() {
}

int G711u::getDelay() {
	return 20;
}

int G711u::getFrameCount() {
	return 160;
}

int G711u::getFrameSize() {
	return 2;
}

float G711u::getFrequency() {
	return 8000.0;
}

int G711u::encode(char *dest, char *src) {
::	ulaw_compress(getFrameCount(), (short *)src, buf);
	for (int i = 0; i < getFrameCount(); ++i) {
		((unsigned char *) dest)[i] = ((unsigned short *)(buf))[i] & 0xFF;
	}
	return getFrameCount();
}

int G711u::decode(char *dest, char *src, int srcsize)
{
	for (int i = 0; i < srcsize; ++i) {
		buf[i] = *((unsigned char *)(src+i));
	}
	::ulaw_expand(srcsize, buf, (short *) dest);
	return srcsize * 2;
}

G711a::G711a()
{

}

G711a::~G711a()
{
}

int G711a::getDelay()
{
	return 20;
}

int G711a::getFrameCount()
{
	return 160;
}

int G711a::getFrameSize()
{
	return 2;
}

float G711a::getFrequency()
{
	return 8000.0;
}

int G711a::encode(char *dest, char *src)
{
	::alaw_compress(getFrameCount(), (short *)src, buf);
	for (int i = 0; i < getFrameCount(); ++i) {
		((unsigned char *) dest)[i] = ((unsigned short *)(buf))[i] & 0xFF;
	}
	return getFrameCount();
}

int G711a::decode(char *dest, char *src, int srcsize)
{
	for (int i = 0; i < srcsize; ++i) {
		buf[i] = *((unsigned char *)(src+i));
	}
	::alaw_expand(srcsize, buf, (short *) dest);
	return 320;
}

} /* namespace */