// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGameStartupSettings_H__
#define __WmeGameStartupSettings_H__


namespace Wme
{
	class WmeDllExport GameStartupSettings
	{
	public:
		GameStartupSettings();
		~GameStartupSettings();

		size_t ResolutionWidth;
		size_t ResolutionHeight;
		int MultisampleLevel;
		bool ColorDepth16bit;
		WideString Title;
		bool FullScreen;
	};
}

#endif // __WmeGameStartupSettings_H__
