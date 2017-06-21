// 
// 
// 

#include "Comm_Device_Driver.h"

Comm_Device_Driver::Comm_Device_Driver()
{
		__control_list = new Vector <Control*>;
}

void Comm_Device_Driver::SetControls(Vector <Control*> *_Controls)
{
	__control_list = _Controls;
}
