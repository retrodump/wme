// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt


#ifndef __WmeUiElement_H__
#define __WmeUiElement_H__


#include "UiObjectBase.h"


namespace Wme
{
	class WmeDllExport UiElement : public UiObjectBase
	{
	public:
		UiElement(Canvas2D* canvas);
		virtual ~UiElement();
	};
}

#endif // __WmeUiElement_H__
