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

#ifndef __GLOBALS_H__INCLUDED__
#define __GLOBALS_H__INCLUDED__

namespace agh {

/** According to RFC3551
 *  See http://www.ietf.org/rfc/rfc3551.txt
 */
struct AudioCodec {
	enum {
		PCMU = 0,
		RESERVED1,
		RESERVED2,
		GSM,
		G723,
		DVI4_8000,
		DVI4_16000,
		LPC,
		PCMA,
		G722,
		L16_STEREO,
		L16_MONO,
		QCELP,
		CN,
		MPA,
		G728,
		DVI4_11025,
		DVI4_22050,
		G729,
		RESERVED3,
		ILBC_20 = 100,
  		DUMMY
	};
};

//extern int audioCodecClockRate[];

//extern int audioCodecChannels[];

} /* namespace agh */

#endif /* __GLOBALS_H__INCLUDED__ */
