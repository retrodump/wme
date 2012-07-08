// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiInteractiveArea_H__
#define __WmeUiInteractiveArea_H__


#include "UiElement.h"


namespace Wme
{
	class InteractiveElement2D;

	class WmeDllExport UiInteractiveArea : public UiElement
	{
	public:
		UiInteractiveArea(Canvas2D* canvas);
		virtual ~UiInteractiveArea();

		// ScriptableObject
		RTTI(UiInteractiveArea);
		virtual WideString ConvertToString() { return L"Interactive area object"; }
		virtual WideString GetTypeName() const { return L"InteractiveArea"; }

		virtual void OnSizeChanged();
		virtual void OnParentChanged();

	private:
		InteractiveElement2D* m_Area;
	};
}

#endif // __WmeUiInteractiveArea_H__
