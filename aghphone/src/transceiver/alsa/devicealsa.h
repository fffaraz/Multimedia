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

#ifndef __DEVICEALSA_H__INCLUDED__
#define __DEVICEALSA_H__INCLUDED__

#include "device.h"
#include <vector>
#include <string>

using namespace std;

namespace agh {
	
class DeviceAlsa : public IDevice {
private:
	string name;
	const vector<double> sampleRates;
public:
	DeviceAlsa(string name) { this->name = name; }

	const string& getName() const { return (const string&)name; }
	int getID() const { return 0; }
	const string& getHostAPI() const { return ""; }
	
	double getDefaultLowInputLatency() const { return 0.0; }
	double getDefaultLowOutputLatency() const { return 0.0; }
	double getDefaultHighInputLatency() const { return 0.0; }
	double getDefaultHighOutputLatency() const { return 0.0; }
	double getDefaultSampleRate() const { return 0.0; }
	const vector<double>& getSupportedSampleRatesHalfDuplexInput() const { return (const vector<double>&)sampleRates; }
	const vector<double>& getSupportedSampleRatesHalfDuplexOutput() const { return (const vector<double>&)sampleRates; }
	const vector<double>& getSupportedSampleRatesFullDuplex() const { return (const vector<double>&)sampleRates; }
	
	int getSupportedChannelCountOutput() const { return 0; }
	int getSupportedChannelCountInput() const { return 0; }
};

} /* namespace agh */

#endif
