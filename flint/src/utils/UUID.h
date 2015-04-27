/*
 * UUID.h
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#ifndef UUID_H_
#define UUID_H_

#include <string>

namespace flint {

class UUID {
public:
	UUID();

	virtual ~UUID();

public:
	static std::string generateUUID();
};

} /* namespace flint */
#endif /* UUID_H_ */
