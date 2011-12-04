// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "GameStartupSettings.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
GameStartupSettings::GameStartupSettings()
{
	ResolutionWidth = 800;
	ResolutionHeight = 600;
	MultisampleLevel = 2;
	Title = L"Wintermute Engine";
	ColorDepth16bit = false;
	FullScreen = false;
}

//////////////////////////////////////////////////////////////////////////
GameStartupSettings::~GameStartupSettings()
{
}


} // namespace Wme
