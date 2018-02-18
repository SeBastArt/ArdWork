/*
* Creator.h
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
#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <string>

class Device_Driver;
class Module_Driver;

class Creator
{
public:
	Creator(const std::string& classname);
	virtual ~Creator() {};

	virtual Device_Driver* create(Module_Driver* module) = 0;
};

#endif //_CREATOR_H_

