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

#include "codecfactory.h"
#include "dummycodec.h"
#include "g711.h"
#include "gsm.h"
#include "ilbc20.h"
#include "globals.h"

using namespace std;

namespace agh {

CodecFactory::CodecFactory()
{
}

CodecFactory::~CodecFactory()
{
}

vector<int> CodecFactory::getSupportedCodecs() {
	vector<int> codecList;
	
	codecList.push_back(AudioCodec::PCMU);
	codecList.push_back(AudioCodec::PCMA);
	codecList.push_back(AudioCodec::GSM);
	codecList.push_back(AudioCodec::ILBC_20);
	codecList.push_back(AudioCodec::DUMMY);
	
	return codecList;
}

string CodecFactory::getCodecName(int id) {
	std::string codecName = "aa";
	
	switch(id) {
		case AudioCodec::PCMU:
			codecName =  "G711u";
			break;
	
		case AudioCodec::PCMA:
			codecName =  "G711a";
			break;
		
		case AudioCodec::GSM:
			codecName =  "GSM";
			break;
		
		case AudioCodec::ILBC_20:
			codecName =  "ILBC20";
			break;
		
		default:
			codecName =  "DUMMY";
			break;
	}
	
	return codecName;
}

Codec *CodecFactory::getCodec(int id)
{
	Codec *retCodec = NULL;
	
	switch(id) {
	case AudioCodec::PCMU:
		retCodec = new G711u();
		break;
	
	case AudioCodec::PCMA:
		retCodec = new G711a();
		break;
		
	case AudioCodec::GSM:
		retCodec = new GSM();
		break;
		
	case AudioCodec::ILBC_20:
		retCodec = new ILBC20();
		break;
		
	default:
		retCodec = new DummyCodec();
		break;
	}
	
	return retCodec;
}

} /* namespace agh */
