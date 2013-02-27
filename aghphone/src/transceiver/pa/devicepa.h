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

#ifndef __DEVICEPA_H__INCLUDED__
#define __DEVICEPA_H__INCLUDED__

#include "device.h"
#include <string>
#include <vector>

using namespace std;

namespace agh {
	
class DevicePa : public IDevice {
private:
	string name;
	string hostAPI;
	int id;
	double defLowInputLat, defLowOutputLat, defHighInputLat, defHighOutputLat;
	double defSampleRate;
	vector<double> sampleRatesHalfDupInput, sampleRatesHalfDupOutput,
		sampleRatesFullDup;
	int channelCountOutput, channelCountInput;
public:
	DevicePa(int index);

	const string& getName() const { return (const string&)name; }
	int getID() const { return id; }
	const string& getHostAPI() const { return (const string&)hostAPI; }
	
	double getDefaultLowInputLatency() const { return defLowInputLat; }
	double getDefaultLowOutputLatency() const { return defLowOutputLat; }
	double getDefaultHighInputLatency() const { return defHighInputLat; }
	double getDefaultHighOutputLatency() const { return defHighOutputLat; }
	double getDefaultSampleRate() const { return defSampleRate; }
	const vector<double>& getSupportedSampleRatesHalfDuplexInput() const { return (const vector<double>&)sampleRatesHalfDupInput; }
	const vector<double>& getSupportedSampleRatesHalfDuplexOutput() const { return (const vector<double>&)sampleRatesHalfDupOutput; }
	const vector<double>& getSupportedSampleRatesFullDuplex() const { return (const vector<double>&)sampleRatesFullDup; }
	
	int getSupportedChannelCountOutput() const { return channelCountOutput; }
	int getSupportedChannelCountInput() const { return channelCountInput; }
};

} /* namespace agh */

#endif
