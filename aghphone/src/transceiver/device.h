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

#ifndef __DEVICE_H__INCLUDED__
#define __DEVICE_H__INCLUDED__

#include <string>
#include <vector>

using namespace std;

namespace agh {
	
class IDevice {
public:
	virtual ~IDevice() {}
	virtual const string& getName() const = 0;
	virtual int getID() const = 0;
	virtual const string& getHostAPI() const = 0;

	virtual double getDefaultLowInputLatency() const = 0;
	virtual double getDefaultLowOutputLatency() const = 0;
	virtual double getDefaultHighInputLatency() const = 0;
	virtual double getDefaultHighOutputLatency() const = 0;
	virtual double getDefaultSampleRate() const = 0;
	virtual const vector<double>& getSupportedSampleRatesHalfDuplexInput() const = 0;
	virtual const vector<double>& getSupportedSampleRatesHalfDuplexOutput() const = 0;
	virtual const vector<double>& getSupportedSampleRatesFullDuplex() const = 0;

	virtual int getSupportedChannelCountOutput() const = 0;
	virtual int getSupportedChannelCountInput() const = 0;
};

} /* namespace agh */

#endif
