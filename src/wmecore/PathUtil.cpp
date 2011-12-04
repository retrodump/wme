// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PathUtil.h"
#include "StringUtil.h"
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

#ifdef _WIN32
#	include <shlobj.h>
#endif

using namespace boost::filesystem;

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
WideString PathUtil::UnifySeparators(const WideString& path)
{
	WideString newPath = path;

	std::replace(newPath.begin(), newPath.end(), L'\\', L'/');
	return newPath;
}

//////////////////////////////////////////////////////////////////////////
WideString PathUtil::NormalizeFileName(const WideString& path)
{
	WideString newPath = UnifySeparators(path);
	StringUtil::ToLowerCase(newPath);
	return newPath;
}

//////////////////////////////////////////////////////////////////////////
WideString PathUtil::Combine(const WideString& path1, const WideString& path2)
{
	WideString newPath1 = UnifySeparators(path1);
	WideString newPath2 = UnifySeparators(path2);

	if (!StringUtil::EndsWith(newPath1, L"/", true) && !StringUtil::StartsWith(newPath2, L"/", true))
		newPath1 += L"/";

	return newPath1 + newPath2;
}

//////////////////////////////////////////////////////////////////////////
WideString PathUtil::GetDirectoryName(const WideString& path)
{
	WideString newPath = UnifySeparators(path);
	
	size_t pos = newPath.find_last_of(L'/');
	
	if (pos == WideString::npos) return L"";
	else return newPath.substr(0, pos + 1);
}

//////////////////////////////////////////////////////////////////////////
WideString PathUtil::GetFileName(const WideString& path)
{
	WideString newPath = UnifySeparators(path);

	size_t pos = newPath.find_last_of(L'/');

	if (pos == WideString::npos) return path;
	else return newPath.substr(pos + 1);
}

//////////////////////////////////////////////////////////////////////////
WideString PathUtil::GetFileNameWithoutExtension(const WideString& path)
{
	WideString fileName = GetFileName(path);

	size_t pos = fileName.find_last_of(L'.');

	if (pos == WideString::npos) return fileName;
	else return fileName.substr(0, pos);
}

//////////////////////////////////////////////////////////////////////////
WideString PathUtil::GetExtension(const WideString& path)
{
	WideString fileName = GetFileName(path);

	size_t pos = fileName.find_last_of(L'.');

	if (pos == WideString::npos) return L"";
	else return fileName.substr(pos);
}


//////////////////////////////////////////////////////////////////////////
bool PathUtil::CreateDirectory(const WideString& path)
{
	try
	{
		return create_directories(path);
	}
	catch (...)
	{
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////
WideString PathUtil::GetSafeLogFileName()
{
	WideString logFileName;

#ifdef _WIN32
	LPITEMIDLIST pidl = NULL;
	LPMALLOC pMalloc;
	wchar_t pathBuf[MAX_PATH];
	pathBuf[0] = L'\0';
	
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl);
		if (pidl)
		{
			SHGetPathFromIDListW(pidl, pathBuf);
		}
		pMalloc->Free(pidl);
	}
	wchar_t moduleName[MAX_PATH];
	::GetModuleFileNameW(NULL, moduleName, MAX_PATH);
	
	logFileName = GetFileNameWithoutExtension(WideString(moduleName)) + L".log";
	logFileName = Combine(L"/Wintermute Engine/Logs/", logFileName);
	logFileName = Combine(pathBuf, logFileName);
	
#else
	// !PORTME
	logFileName = L"./wme.log";
#endif

	CreateDirectory(GetDirectoryName(logFileName));
	return logFileName;
}

//////////////////////////////////////////////////////////////////////////
bool PathUtil::MatchesMask(const WideString& fileName, const WideString& mask)
{
	WideString pattern = mask;
	pattern = StringUtil::Replace(pattern, L".", L"\\.");
	pattern = StringUtil::Replace(pattern, L"?", L".");
	pattern = StringUtil::Replace(pattern, L"*", L".*");

	boost::wregex exp(pattern, boost::regex::icase);
	return boost::regex_match(fileName, exp);
}

//////////////////////////////////////////////////////////////////////////
bool PathUtil::FileExists(const WideString& fileName)
{
	std::ifstream stream;
#ifdef WIN32
	stream.open(fileName.c_str());
#else
	stream.open(StringUtil::WideToAnsi(fileName).c_str());
#endif
	bool ret = stream.is_open();
	stream.close();

	return ret;
}


} // namespace Wme
