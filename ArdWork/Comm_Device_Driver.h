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

class Comm_Device_Driver 
{
private:
	void SetControls(Vector <Control*> *_Controls);
protected:
	Vector <Control*> *__control_list;
public:
	Comm_Device_Driver();
	virtual void UpdateControls() = 0;
	Property<Vector <Control*>*, Comm_Device_Driver> control_list{ this,&Comm_Device_Driver::SetControls,nullptr };
};

#endif
