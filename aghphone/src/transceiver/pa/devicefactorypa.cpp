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

#include "devicefactorypa.h"
#include <portaudio.h>
#include <iostream>

using namespace std;

namespace agh {

DeviceFactoryPa::DeviceFactoryPa()
{
	deviceCount = Pa_GetDeviceCount();
	
	for(int i=0; i<deviceCount; i++)
		devs.push_back( new DevicePa(i) );
	
	cout << "Found " << deviceCount << " supported audio devices." << endl;
}

IDevice& DeviceFactoryPa::getDefaultInputDevice() const
{
	return getDevice(Pa_GetDefaultInputDevice());
}

IDevice& DeviceFactoryPa::getDefaultOutputDevice() const
{
	return getDevice(Pa_GetDefaultOutputDevice());
}

}
