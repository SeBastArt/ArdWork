// Comm_Device_Driver.h

#ifndef _COMM_DEVICE_DRIVER_h
#define _COMM_DEVICE_DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Base_Consts.h"
#include "m_Vector.h"
#include "Publisher.h"

class Comm_Device_Driver : public Observer
{
private:
	Comm_Device_Driver(const Comm_Device_Driver& yRef) = delete;
	Comm_Device_Driver& operator=(const Comm_Device_Driver& yRef) = delete;
protected:
	Vector <Publisher*> *__pub_list;
public:
	Comm_Device_Driver() {__pub_list = new Vector <Publisher*>;};
	virtual void Notify(Vector <Publisher*> *pub_list) { __pub_list = pub_list; };
};

#endif
