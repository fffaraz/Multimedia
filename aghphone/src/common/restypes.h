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

#ifndef __RESTYPES_H__INCLUDED__
#define __RESTYPES_H__INCLUDED__

namespace agh {

struct RequestConnectResult {
	enum {
		SUCCESS = 0,
		INTERNAL_ERROR,
		NOT_IMPLEMENED_YET,
		BUSY,
		REJECTED,
		
		OTHER = -1
	};
};

struct SetCodecResult {
	enum {
		SUCCESS = 0,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED_YET,
		NO_SUCH_CODEC,
		NOT_SUPPORTED,
		
		OTHER = -1
	};
};

struct SetDestinationPortResult {
	enum {
		SUCCESS = 0,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED_YET,
		OUT_OF_RANGE,
		
		OTHER = -1
	};
};

struct StartTransmissionResult {
	enum {
		SUCCESS = 0,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED_YET,
		NOT_CONNECTED,
		
		OTHER = -1
	};
};

struct DisconnectResult {
	enum {
		SUCCESS = 0,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED_YET,
		
		OTHER = -1
	};
};

struct DisconnectReason {
	enum {
		USER_REQUESTED,
		POOR_QUALITY,
		
		OTHER = -1
	};
};

struct ConnectResult {
	enum {
		SUCCESS = 0,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED_YET,
		INVALID_HOST,
		ALREADY_CONNECTED,
		REJECTED,
		BUSY,
		
		OTHER = -1
	};
};

struct NegotiateResult {
	enum {
		SUCCESS,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED_YET,
		MUTUALLY_DISCRAPENCY,
		
		OTHER = -1
	};
};

struct TransceiverStartResult {
	enum {
		SUCCESS = 0,
		LOCAL_ENDPOINT_ERROR,
		REMOTE_ENDPOINT_ERROR,
		STREAM_OPEN_ERROR,
		
		OTHER = -1
	};
};

}

#endif /* __RESTYPES_H__INCLUDED__ */
