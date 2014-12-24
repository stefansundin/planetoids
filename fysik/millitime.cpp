#include "millitime.h"
#include <windows.h>
//#include <iostream>

//long mytime=0;

long millitime()
{
	/*std::cout << GetTickCount() << "\n";
	return GetTickCount();*/
	/*long t;
	struct timeval tv;

	gettimeofday(&tv, 0);

	t = tv.tv_sec;
	t = (t*1000) + (tv.tv_usec/1000);

	return t;*/
	/*mytime+=1000;
	return mytime;*/

	SYSTEMTIME tv;
	GetSystemTime(&tv);
	long t=/*(tv.wYear-1980)*365*24*60*60*1000+*/tv.wMonth*12*24*60*60*1000+tv.wDay*24*60*60*1000+tv.wHour*60*60*1000+tv.wMinute*60*1000+tv.wSecond*1000+tv.wMilliseconds;
	//std::cout << t << "\n";
	if(t<0){
		t*=-1;
	}
	return t;

}

