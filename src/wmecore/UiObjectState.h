// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObjectState_H__
#define __WmeUiObjectState_H__


#include "UiObjectBase.h"


namespace Wme
{
	class WmeDllExport UiObjectState : public UiObjectBase
	{
	public:
		UiObjectState(Canvas2D* canvas);
		virtual ~UiObjectState();
	};
}

#endif // __WmeUiObjectState_H__
