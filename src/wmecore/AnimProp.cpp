// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimProp.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimProp::AnimProp()
{
	Mode = ANIM_CROSSFADE;
	Name = L"";
	Layer = 0;
	FadeInTime = 200;
	FadeOutTime = 200;
	AnimWhileFadingIn = true;
	AnimWhileFadingOut = true;
	AnimPrevWhileFadingIn = true;
}

//////////////////////////////////////////////////////////////////////////
AnimProp::~AnimProp()
{
}


} // namespace Wme
