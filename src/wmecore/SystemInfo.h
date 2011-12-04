// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSystemInfo_H__
#define __WmeSystemInfo_H__


namespace Wme
{
	class WmeDllExport SystemInfo
	{
	public:
		static const WideStringSet& GetSupportedImageFormats();

	private:
		static WideStringSet s_SupportedImageFormats;
	};
}


#endif // __WmeSystemInfo_H__