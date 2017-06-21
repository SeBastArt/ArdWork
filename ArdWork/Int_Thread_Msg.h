// MessageQ.h

#ifndef _MESSAGE_h
#define _MESSAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Int_Thread_Msg
{
private:
	String ParamString;
	String GetStringPartByNr(String data, char separator, int index);
public:
	Int_Thread_Msg(int ID);
	void AddParam(int intNumber);
	void AddParam(unsigned char c);
	void AddParam(String text);
	void AddParam(float floatNumber);
	int GetID();
	int GetIntParamByIndex(int index);
	float GetFloatParamByIndex(int index);
	unsigned char GetCharParamByIndex(int index);
	String GetStringParamByIndex(int index);
};


#endif

