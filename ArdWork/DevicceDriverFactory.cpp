/*
* Factory.cpp
*
*  Created on: April 14, 2013
*      Author: David J. Rager
*       Email: djrager@fourthwoods.com
*
* This code is hereby released into the public domain per the Creative Commons
* Public Domain dedication.
*
* http://http://creativecommons.org/publicdomain/zero/1.0/
*/
#include "DeviceDriverFactory.h"
#include "Creator.h"

void DeviceDriverFactory::registerit(const std::string& classname, Creator* creator)
{
	get_table()[classname] = creator;
}

Driver* DeviceDriverFactory::create(const std::string& classname)
{
	for (std::map<std::string, Creator*>::iterator it = get_table().begin(); it != get_table().end(); ++it) {
		if (it->first.compare(classname))
			return it->second->create();
	}
	return (Driver*)NULL;
}

std::map<std::string, Creator*>& DeviceDriverFactory::get_table()
{
	static std::map<std::string, Creator*> table;
	return table;
}

