// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiControl_H__
#define __WmeUiControl_H__


#include "UiElement.h"


namespace Wme
{
	class WmeDllExport UiControl : public UiElement
	{
	public:
		UiControl(Canvas2D* canvas);
		virtual ~UiControl();

		virtual bool IsControl() const { return true; }

	protected:

	};
}

#endif // __WmeUiControl_H__