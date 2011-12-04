// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimProp_H__
#define __WmeAnimProp_H__


namespace Wme
{
	struct WmeDllExport AnimProp
	{
		AnimProp();
		~AnimProp();

		enum StartMode
		{
			ANIM_START,
			ANIM_CROSSFADE,
			ANIM_FORCE_CROSSFADE // will start from start even if it's already playing
		};

		StartMode Mode;
		WideString Name;
		size_t Layer;
		unsigned long FadeInTime;
		unsigned long FadeOutTime;
		bool AnimWhileFadingIn;
		bool AnimWhileFadingOut;
		bool AnimPrevWhileFadingIn;
	};
}

#endif // __WmeAnimProp_H__
