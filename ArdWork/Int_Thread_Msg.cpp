// 
// 
// 

#include "Int_Thread_Msg.h"

Int_Thread_Msg::Int_Thread_Msg(uint32 ID) {
	ParamString = "";
	ParamString += ID;
	ParamString += ":";
}

void Int_Thread_Msg::AddParam(bool boolean)
{
	ParamString += boolean;
	ParamString += ":";
}

void Int_Thread_Msg::AddParam(int intNumber) {
	ParamString += intNumber;
	ParamString += ":";
}

void Int_Thread_Msg::AddParam(unsigned char c) {
	ParamString += char(c);
	ParamString += ":";
}

void Int_Thread_Msg::AddParam(String text) {
	ParamString += text;
	ParamString += ":";
}

void Int_Thread_Msg::AddParam(float floatNumber) {
	ParamString += floatNumber;
	ParamString += ":";
}

int Int_Thread_Msg::GetID() {
	int i = 0;
	String ParamStr;
	ParamStr = GetStringPartByNr(ParamString, ':', 0);
	i = ParamStr.toInt();
	return i;
}

int Int_Thread_Msg::GetIntParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(ParamString, ':', index);
	return ParamStr.toInt();
}
bool Int_Thread_Msg::GetBoolParamByIndex(int index) {
	String ParamStr;
	bool tempResult = false;
	ParamStr = GetStringPartByNr(ParamString, ':', index);
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
	ParamStr = GetStringPartByNr(ParamString, ':', index);
	return ParamStr.toFloat();
}

String Int_Thread_Msg::GetStringParamByIndex(int index) {
	String ParamStr;
	ParamStr = GetStringPartByNr(ParamString, ':', index);
	return ParamStr;
}

unsigned char Int_Thread_Msg::GetCharParamByIndex(int index) {
	String ParamStr;
	unsigned char result;
	ParamStr = GetStringPartByNr(ParamString, ':', index);
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

