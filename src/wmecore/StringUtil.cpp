// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "wme.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include "StringUtil.h"
#include "ConvertUTF.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
void StringUtil::ToLowerCase(AnsiString& str)
{
	std::transform(str.begin(), str.end(), str.begin(),	tolower);
}

//////////////////////////////////////////////////////////////////////////
void StringUtil::ToLowerCase(WideString& str)
{
	std::transform(str.begin(), str.end(), str.begin(),	towlower);
}

//////////////////////////////////////////////////////////////////////////
void StringUtil::ToUpperCase(AnsiString& str)
{
	std::transform(str.begin(),	str.end(), str.begin(), toupper);
}

//////////////////////////////////////////////////////////////////////////
void StringUtil::ToUpperCase(WideString& str)
{
	std::transform(str.begin(),	str.end(), str.begin(), towupper);
}

//////////////////////////////////////////////////////////////////////////
bool StringUtil::CompareNoCase(const AnsiString& str1, const AnsiString& str2)
{
	AnsiString str1lc = str1;
	AnsiString str2lc = str2;

	ToLowerCase(str1lc);
	ToLowerCase(str2lc);

	return (str1lc == str2lc);
}

//////////////////////////////////////////////////////////////////////////
bool StringUtil::CompareNoCase(const WideString& str1, const WideString& str2)
{
	WideString str1lc = str1;
	WideString str2lc = str2;

	ToLowerCase(str1lc);
	ToLowerCase(str2lc);

	return (str1lc == str2lc);
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::Utf8ToWide(const Utf8String& Utf8Str)
{
	size_t WideSize = Utf8Str.length();
	
	if (sizeof(wchar_t) == 2)
	{
		wchar_t* WideStringNative = new wchar_t[WideSize+1];
		
		const UTF8* SourceStart = reinterpret_cast<const UTF8*>(Utf8Str.c_str());
		const UTF8* SourceEnd = SourceStart + WideSize;
		
		UTF16* TargetStart = reinterpret_cast<UTF16*>(WideStringNative);
		UTF16* TargetEnd = TargetStart + WideSize+1;
		
		ConversionResult res = ConvertUTF8toUTF16(&SourceStart, SourceEnd, &TargetStart, TargetEnd, strictConversion);
		if (res != conversionOK)
		{
			delete [] WideStringNative;
			return L"";
		}
		*TargetStart = 0;
		WideString ResultString(WideStringNative);
		delete [] WideStringNative;
		
		return ResultString;
	}
	else if (sizeof(wchar_t) == 4)
	{
		wchar_t* WideStringNative = new wchar_t[WideSize+1];
	
		const UTF8* SourceStart = reinterpret_cast<const UTF8*>(Utf8Str.c_str());
		const UTF8* SourceEnd = SourceStart + WideSize;

		UTF32* TargetStart = reinterpret_cast<UTF32*>(WideStringNative);
		UTF32* TargetEnd = TargetStart + WideSize;

		ConversionResult res = ConvertUTF8toUTF32(&SourceStart, SourceEnd, &TargetStart, TargetEnd, strictConversion);
		if (res != conversionOK)
		{
			delete [] WideStringNative;
			return L"";
		}
		*TargetStart = 0;
		WideString ResultString(WideStringNative);
		delete [] WideStringNative;
		
		return ResultString;
	}
	else
	{
		return L"";
	}
}

//////////////////////////////////////////////////////////////////////////
Utf8String StringUtil::WideToUtf8(const WideString& WideStr)
{
	size_t WideSize = WideStr.length();

	if (sizeof(wchar_t) == 2)
	{
		size_t Utf8Size = 3 * WideSize + 1;
		char* Utf8StringNative = new char[Utf8Size];
		
		const UTF16* SourceStart = reinterpret_cast<const UTF16*>(WideStr.c_str());
		const UTF16* SourceEnd = SourceStart + WideSize;
		
		UTF8* TargetStart = reinterpret_cast<UTF8*>(Utf8StringNative);
		UTF8* TargetEnd = TargetStart + Utf8Size;
		
		ConversionResult res = ConvertUTF16toUTF8(&SourceStart, SourceEnd, &TargetStart, TargetEnd, strictConversion);
		if (res != conversionOK)
		{
			delete [] Utf8StringNative;
			return (Utf8String)"";
		}
		*TargetStart = 0;
		Utf8String ResultString(Utf8StringNative);
		delete [] Utf8StringNative;
		return ResultString;
	}
	else if (sizeof(wchar_t) == 4)
	{
		size_t Utf8Size = 4 * WideSize + 1;
		char* Utf8StringNative = new char[Utf8Size];
		
		const UTF32* SourceStart = reinterpret_cast<const UTF32*>(WideStr.c_str());
		const UTF32* SourceEnd = SourceStart + WideSize;
		
		UTF8* TargetStart = reinterpret_cast<UTF8*>(Utf8StringNative);
		UTF8* TargetEnd = TargetStart + Utf8Size;
		
		ConversionResult res = ConvertUTF32toUTF8(&SourceStart, SourceEnd, &TargetStart, TargetEnd, strictConversion);
		if (res != conversionOK)
		{
			delete [] Utf8StringNative;
			return (Utf8String)"";
		}
		*TargetStart = 0;
		Utf8String ResultString(Utf8StringNative);
		delete [] Utf8StringNative;
		return ResultString;
	}
	else
	{
		return (Utf8String)"";
	}
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::AnsiToWide(const AnsiString& str)
{
	// using default os locale!
	setlocale(LC_CTYPE, "");
	size_t WideSize = mbstowcs(NULL, str.c_str(), 0) + 1;
	wchar_t *wstr = new wchar_t[WideSize];
	mbstowcs(wstr, str.c_str(), WideSize);
	WideString ResultString(wstr);
	delete [] wstr;
	return ResultString;
}

//////////////////////////////////////////////////////////////////////////
AnsiString StringUtil::WideToAnsi(const WideString& wstr)
{
	// using default os locale!
	setlocale(LC_CTYPE, "");
	size_t WideSize = wcstombs(NULL, wstr.c_str(), 0) + 1;
	char *str = new char[WideSize];
	wcstombs(str, wstr.c_str(), WideSize);
	AnsiString ResultString(str);
	delete [] str;
	return ResultString;
}

//////////////////////////////////////////////////////////////////////////
bool StringUtil::StartsWith(const WideString& str, const WideString& pattern, bool ignoreCase)
{
	size_t strLength = str.length();
	size_t patternLength = pattern.length();
	
	if (strLength < patternLength || patternLength == 0)
		return false;

	WideString startPart = str.substr(0, patternLength);

	if (ignoreCase) return CompareNoCase(startPart, pattern);
	else return (startPart == pattern);
}

//////////////////////////////////////////////////////////////////////////
bool StringUtil::EndsWith(const WideString& str, const WideString& pattern, bool ignoreCase)
{
	size_t strLength = str.length();
	size_t patternLength = pattern.length();

	if (strLength < patternLength || patternLength == 0)
		return false;

	WideString endPart = str.substr(strLength - patternLength, patternLength);

	if (ignoreCase) return CompareNoCase(endPart, pattern);
	else return (endPart == pattern);
}

//////////////////////////////////////////////////////////////////////////
WideStringListPtr StringUtil::Split(const WideString& list, const WideString& delimiters, bool keepEmptyItems)
{
	WideStringListPtr retList = WideStringListPtr(new WideStringList());

	typedef boost::char_separator<wchar_t> separator_t;
	typedef boost::tokenizer<separator_t, WideString::const_iterator, WideString> tokenizer_t;
	
	separator_t del(delimiters.c_str(), L"", keepEmptyItems ? boost::keep_empty_tokens : boost::drop_empty_tokens);
	tokenizer_t tokens(list, del);
	for (tokenizer_t::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		retList->push_back(*it);
	}
	return retList;
}

//////////////////////////////////////////////////////////////////////////
bool StringUtil::IsUtf8BOM(const byte* Buffer, dword BufferSize)
{
	if (BufferSize > 3 && Buffer[0]==0xEF && Buffer[1]==0xBB && Buffer[2]==0xBF) return true;
	else return false;
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::Replace(const WideString& str, const WideString& from, const WideString& to)
{
	if (from.empty() || from == to) return str;

	WideString result = str;
	size_t pos = 0;

	while (true)
	{
		pos = result.find(from, pos);
		if (pos == result.npos) break;

		result.replace(pos, from.size(), to);
		pos += to.size();
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::Trim(const WideString& str, bool fromLeft, bool fromRight, const WideString& chars)
{
	WideString trimmedStr = str;

	if (fromRight)
		trimmedStr.erase(trimmedStr.find_last_not_of(chars) + 1);
	if (fromLeft)
		trimmedStr.erase(0, trimmedStr.find_first_not_of(chars));

	return trimmedStr;
}

//////////////////////////////////////////////////////////////////////////
int StringUtil::IndexOf(const WideString& str, const WideString& toFind, size_t startFrom)
{
	size_t pos = str.find(toFind, startFrom);
	if (pos == str.npos) return -1;
	else return pos;
}

//////////////////////////////////////////////////////////////////////////
int StringUtil::LastIndexOf(const WideString& str, const WideString& toFind, size_t startFrom)
{
	size_t pos = str.rfind(toFind, startFrom);
	if (pos == str.npos) return -1;
	else return pos;
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::ToString(size_t val)
{
	std::wostringstream str;
	str << val;
	return str.str();
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::ToString(int val)
{
	std::wostringstream str;
	str << val;
	return str.str();
	
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::ToString(float val)
{
	std::wostringstream str;
	str << val;
	return str.str();
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::ToString(double val)
{
	std::wostringstream str;
	str << val;
	return str.str();
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::ToString(const Ogre::Vector3& val)
{
	return ToString(val.x) + L", " + ToString(val.y) + L", " + ToString(val.z);
}


} // namespace Wme
