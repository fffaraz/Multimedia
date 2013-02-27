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

#ifndef __DEVICEFACTORYALSA_H__INCLUDED__
#define __DEVICEFACTORYALSA_H__INCLUDED__

#include "device.h"
#include "devicealsa.h"
#include "devicefactory.h"
#include <vector>
#include <map>

using namespace std;

namespace agh {
	
class DeviceFactoryAlsa : public IDeviceFactory {
	IDevice *defaultDevice;
	vector<DeviceAlsa*> devs;
	map<string, DeviceAlsa*> devmap;
public:
	DeviceFactoryAlsa()
	{
		DeviceAlsa *dev = new DeviceAlsa("default");
		devs.push_back(dev);
		devmap["default"] = dev;
		defaultDevice = dev;
	}
	
	int getDeviceCount() const { return devs.size(); }
	IDevice& getDevice(int index) const { return (IDevice& )(*devs[index]); }
	IDevice& getDevice(const string name) { 
		map<string, DeviceAlsa*>::iterator iter; 
		if((iter = devmap.find(name)) != devmap.end())
			return *((*iter).second);
		 else {
		 	DeviceAlsa *dev = new DeviceAlsa(name);
		 	devmap[name] = dev;
		 	devs.push_back(dev);
		 	return *dev;
		 }
	}
	
	IDevice& getDefaultInputDevice() const { return *defaultDevice; }
	IDevice& getDefaultOutputDevice() const { return *defaultDevice; }
};

} /* namespace agh */

#endif
