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

#ifndef __CODEC_H__INCLUDED__
#define __CODEC_H__INCLUDED__

namespace agh {

class Codec {
public:
	virtual ~Codec() {}
	
	
	/**
	 * @return time between sending each packet [ms]
	 */
	virtual int getDelay() = 0;
	
	/**
	 * @return number of frames in each packet, one frame contain one sample(in single channel stream)
	 */
	
	virtual int getFrameCount() = 0;
	
	/**
	 * @return size of one frame [B]
	 */
	
	virtual int getFrameSize() = 0;
	
	/**
	 * @return sampling frequency [Hz]
	 */
	
	virtual float getFrequency() = 0;
	
	/**
	 * @param[out] dest pointer to a buffer to which encoded packet will be written
	 * @param[in]  src  pointer to a buffer from which data to encode will be read
	 * @return number of bytes written to the destination buffer
	 */
	virtual int encode(char *dest, char *src) = 0;
	
	/**
	 * @param[out] dest pointer to a buffer to which decoded packet will be written
	 * @param[in]  src  pointer to a buffer from which data to decode will be read
	 * @param srcsize	size in bytes of the data in buffer src
	 * @return number of bytes written to the destination buffer
	 */
	virtual int decode(char *dest, char *src, int srcsize) = 0;
};

} /* namespace agh */

#endif /* __CODEC_H__INCLUDED__ */
