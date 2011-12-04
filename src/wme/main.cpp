// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Game.h"

#if defined(_MSC_VER)
#	ifdef _DEBUG
#		include <vld.h>
#	endif
#endif



#if defined(WIN32)
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[]) 
#endif
{
	Wme::Game* Game = Wme::Game::GetInstance();
	if (Game->Initialize(true))
	{
		Game->Run();
		delete Game;
		return 0;
	}
	else
	{
		delete Game;
		return 1;
	}
}
