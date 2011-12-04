// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeInteractiveObject_H__
#define __WmeInteractiveObject_H__


#include "ScriptableObject.h"
#include "IMouseHandler.h"


namespace Wme
{
	class WmeDllExport InteractiveObject : public ScriptableObject, public IMouseHandler
	{
	public:
		InteractiveObject();
		virtual ~InteractiveObject();

		// script event routing
		bool InvokeScriptMouseEvent(Viewport* viewport, MouseEvent& event);

		// events
		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		bool HasMouseCapture() const;
	};
}

#endif // __WmeInteractiveObject_H__
