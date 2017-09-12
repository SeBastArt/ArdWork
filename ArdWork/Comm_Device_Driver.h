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
	Vector <Descriptor*> *__descriptor_list;
public:
	Comm_Device_Driver() { __descriptor_list = new Vector <Descriptor*>;};
	virtual void Notify(Vector <Descriptor*> *_descriptor_list) { __descriptor_list = _descriptor_list; };
};

#endif
