// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeIMouseHandler_H__
#define __WmeIMouseHandler_H__


#include "MouseEvent.h"


namespace Wme
{
	class Viewport;

	class WmeDllExport IMouseHandler
	{
	public:
		virtual ~IMouseHandler() {}

		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event) = 0;
		virtual bool HasMouseCapture() const = 0;
	};
}

#endif // __WmeIMouseHandler_H__
