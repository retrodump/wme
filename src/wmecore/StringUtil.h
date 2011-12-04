// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeStringUtil_H__
#define __WmeStringUtil_H__


#include "WmeTypes.h"


namespace Wme
{
	class WmeDllExport StringUtil
	{
	public:
		static void ToLowerCase(AnsiString& str);
		static void ToLowerCase(WideString& str);
		static void ToUpperCase(AnsiString& str);
		static void ToUpperCase(WideString& str);
		static bool CompareNoCase(const AnsiString& str1, const AnsiString& str2);
		static bool CompareNoCase(const WideString& str1, const WideString& str2);
		static WideString Utf8ToWide(const Utf8String& Utf8Str);
		static Utf8String WideToUtf8(const WideString& WideStr);
		static WideString AnsiToWide(const AnsiString& str);
		static AnsiString WideToAnsi(const WideString& str);

		static bool StartsWith(const WideString& str, const WideString& pattern, bool ignoreCase=false);
		static bool EndsWith(const WideString& str, const WideString& pattern, bool ignoreCase=false);

		static WideStringListPtr Split(const WideString& list, const WideString& delimiters, bool keepEmptyItems=false);
		
		static bool IsUtf8BOM(const byte* buffer, dword bufferSize);

		static WideString Replace(const WideString& str, const WideString& from, const WideString& to);
		static WideString Trim(const WideString& str, bool fromLeft = true, bool fromRight = true, const WideString& chars = L" \n\r\t");

		static int IndexOf(const WideString& str, const WideString& toFind, size_t startFrom);
		static int LastIndexOf(const WideString& str, const WideString& toFind, size_t startFrom);

		static WideString ToString(size_t val);
		static WideString ToString(int val);
		static WideString ToString(float val);
		static WideString ToString(double val);
		static WideString ToString(const Ogre::Vector3& val);
	};
}
#endif // __WmeStringUtil_H__
