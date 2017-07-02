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

class Comm_Device_Driver 
{
private:
	void SetPublisher(Vector <Publisher*> *_publisher);
protected:
	Vector <Publisher*> *__pub_list;
public:
	Comm_Device_Driver();
	virtual void UpdateControls() = 0;
	Property<Vector <Publisher*>*, Comm_Device_Driver> pub_list{ this,&Comm_Device_Driver::SetPublisher,nullptr };
};

#endif
