// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePathUtils_H__
#define __WmePathUtils_H__

namespace Wme
{
	class WmeDllExport PathUtil
	{
	public:
		static WideString UnifySeparators(const WideString& path);
		static WideString NormalizeFileName(const WideString& path);
		static WideString Combine(const WideString& path1, const WideString& path2);
		static WideString GetDirectoryName(const WideString& path);
		static WideString GetFileName(const WideString& path);
		static WideString GetFileNameWithoutExtension(const WideString& path);
		static WideString GetExtension(const WideString& path);
		static bool CreateDirectory(const WideString& path);
		static bool MatchesMask(const WideString& fileName, const WideString& mask);

		static bool FileExists(const WideString& fileName);

		static WideString GetSafeLogFileName();
	};
}

#endif // __WmePathUtils_H__
