// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "DateTimeUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
time_t DateTimeUtil::BuildTime(word year, byte month, byte day, byte hour, byte minute, byte second)
{
	tm time;
	memset(&time, 0, sizeof(tm));

	time.tm_year = year - 1900;
	time.tm_mon = month - 1;
	time.tm_mday = day;
	time.tm_hour = hour;
	time.tm_min = minute;
	time.tm_sec = second;

	return mktime(&time);
}

//////////////////////////////////////////////////////////////////////////
void DateTimeUtil::ParseTime(time_t time, word& year, byte& month, byte& day, byte& hour, byte& minute, byte& second)
{
	struct tm* timeStruct = localtime(&time);
	year = 1900 + timeStruct->tm_year;
	month = 1 + timeStruct->tm_mon;
	day = timeStruct->tm_mday;
	hour = timeStruct->tm_hour;
	minute = timeStruct->tm_min;
	second = timeStruct->tm_sec;
}


} // namespace Wme
