// 
// 
// 

#include "Comm_Device_Driver.h"

Comm_Device_Driver::Comm_Device_Driver()
{
	__pub_list = new Vector <Publisher*>;
}

void Comm_Device_Driver::SetPublisher(Vector <Publisher*> *_publisher)
{
	__pub_list = _publisher;
}