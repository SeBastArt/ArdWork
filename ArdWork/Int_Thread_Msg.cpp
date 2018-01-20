// 
// 
// 

#include "Int_Thread_Msg.h"

Int_Thread_Msg::Int_Thread_Msg(uint32 _id) {
	__id = _id;
	__param_count = 0;
}

void Int_Thread_Msg::AddParam(bool boolean)
{
	__str_param += boolean;
	__str_param += ":";
	__param_count++;
}

void Int_Thread_Msg::AddParam(int intNumber) {
	__str_param += intNumber;
	__str_param += ":";
	__param_count++;
}

void Int_Thread_Msg::AddParam(unsigned char c) {
	__str_param += char(c);
	__str_param += ":";
	__param_count++;
}

void Int_Thread_Msg::AddParam(String text) {
	__str_param += text;
	__str_param += ":";
	__param_count++;
}

void Int_Thread_Msg::AddParam(float floatNumber) {
	__str_param += floatNumber;
	__str_param += ":";
	__param_count++;
}

uint8_t Int_Thread_Msg::GetUint8ParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return (uint8_t)ParamStr.toInt();
}

int Int_Thread_Msg::GetIntParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return ParamStr.toInt();
}
bool Int_Thread_Msg::GetBoolParamByIndex(int index) {
	String ParamStr;
	bool tempResult = false;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	if (ParamStr.toInt() > 0) {
		tempResult = true;
	}
	else {
		tempResult = false;
	}
	return tempResult;
}

float Int_Thread_Msg::GetFloatParamByIndex(int index)
{
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return ParamStr.toFloat();
}

String Int_Thread_Msg::GetStringParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	return ParamStr;
}

uint8_t Int_Thread_Msg::GetCharParamByIndex(int index) {
	String ParamStr;
	unsigned char result;
	ParamStr = GetStringPartByNr(__str_param, ':', index);
	result = (unsigned char)*(ParamStr.c_str());
	return result;
}


String Int_Thread_Msg::GetStringPartByNr(String data, char separator, int index)
{
	// spliting a string and return the part nr index
	// split by separator

	int stringData = 0;        //variable to count data part nr
	String dataPart = "";      //variable to hole the return text

	for (int i = 0; i < data.length(); i++) { //Walk through the text one letter at a time

		if (data[i] == separator) {
			//Count the number of times separator character appears in the text
			stringData++;

		}
		else if (stringData == index) {
			//get the text when separator is the rignt one
			dataPart.concat(data[i]);

		}
		else if (stringData > index) {
			//return text and stop if the next separator appears - to save CPU-time
			return dataPart;
			break;

		}

	}
	//return text if this is the last part
	return dataPart;
}


