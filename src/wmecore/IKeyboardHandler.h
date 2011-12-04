// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeIKeyboardHandler_H__
#define __WmeIKeyboardHandler_H__


#include "KeyboardEvent.h"


namespace Wme
{
	class WmeDllExport IKeyboardHandler
	{
	public:
		virtual ~IKeyboardHandler() {}

		virtual bool HandleKeyboardEvent(KeyboardEvent& event) = 0;
	};
}

#endif // __WmeIMouseHandler_H__
