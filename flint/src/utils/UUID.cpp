/*
 * UUID.cpp
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "UUID.h"

namespace flint {

UUID::UUID() {
	// TODO Auto-generated constructor stub

}

UUID::~UUID() {
	// TODO Auto-generated destructor stub
}

std::string UUID::generateUUID() {
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	std::stringstream ss;
	ss << uuid;
	std::string _uuid = ss.str();
	return _uuid;
}

} /* namespace flint */
