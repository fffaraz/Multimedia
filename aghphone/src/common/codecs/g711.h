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

#ifndef __G711_H__INCLUDED__
#define __G711_H__INCLUDED__

#include "codec.h"

namespace agh {

class G711u : public Codec {
protected:
	short buf[160];
	
public:
	G711u();
	~G711u();
	int getDelay();
	int getFrameCount();
	int getFrameSize();
	float getFrequency();
	int encode(char *dest, char *src);
	int decode(char *dest, char *src, int srcsize);
};

class G711a : public Codec {
protected:
	short buf[160];
	
public:
	G711a();
	~G711a();
	int getDelay();
	int getFrameCount();
	int getFrameSize();
	float getFrequency();
	int encode(char *dest, char *src);
	int decode(char *dest, char *src, int srcsize);
};

} /* namespace agh */

#endif /* __G711_H__INCLUDED__ */
