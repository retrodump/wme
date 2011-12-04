// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeDateTimeUtil_H__
#define __WmeDateTimeUtil_H__

#include <time.h>

namespace Wme
{
	class WmeDllExport DateTimeUtil
	{
	public:
		static time_t BuildTime(word year, byte month, byte day, byte hour=0, byte minute=0, byte second=0);
		static void ParseTime(time_t time, word& year, byte& month, byte& day, byte& hour, byte& minute, byte& second);
	};
}


#endif // __WmeDateTimeUtil_H__