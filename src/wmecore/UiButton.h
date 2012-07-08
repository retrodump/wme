// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiButton_H__
#define __WmeUiButton_H__


#include "UiControl.h"


namespace Wme
{
	class WmeDllExport UiButton : public UiControl
	{
	public:
		UiButton(Canvas2D* canvas);
		virtual ~UiButton();


		bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);

		// ScriptableObject
		RTTI(UiButton);
		virtual WideString ConvertToString() { return L"Button object"; }
		virtual WideString GetTypeName() const { return L"Button"; }

	protected:
		void UpdateState();
		void OnStateChanged();

		bool m_IsBeingPressed;

	};
}

#endif // __WmeWindow_H__
