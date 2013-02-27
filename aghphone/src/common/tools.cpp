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

#include "tools.h"
#include <cc++/socket.h>
#include <Ice/Ice.h>
#include <cstdio>
#include <strstream>
#include <boost/regex.hpp>

using namespace agh;
using namespace ost;
using namespace std;
using namespace boost;

namespace agh {

IPV4Address getRemoteAddressFromConnection(const Ice::ConnectionPtr& con) {

	regex expr("^remote address = ([.0-9]+):[0-9]+");

	string text = con->toString();
	stringstream stream(text);
	string line;

	cmatch what;

	while (getline(stream, line)) {
		if (regex_match(line.c_str(), what, expr)) {
			cout << what[1] << endl;
		}
	}
	return IPV4Address(String(what[1]));
}

IPV4Address getLocalAddressFromConnection(const Ice::ConnectionPtr& con) {

	regex expr("^local address = ([.0-9]+):[0-9]+");

	string text = con->toString();
	stringstream stream(text);
	string line;

	cmatch what;

	while (getline(stream, line)) {
		if (regex_match(line.c_str(), what, expr)) {
			cout << what[1] << endl;
		}
	}
	return IPV4Address(String(what[1]));
}

} /* namespace agh */
