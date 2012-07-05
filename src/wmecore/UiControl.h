// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiControl_H__
#define __WmeUiControl_H__


#include "UiElement.h"


namespace Wme
{
	class UiObjectState;

	class WmeDllExport UiControl : public UiElement
	{
	public:
		UiControl(Canvas2D* canvas);
		virtual ~UiControl();

		WideString GetCurrentState() const { return m_CurrentState; }
		void SetCurrentState(const WideString& stateName);

	protected:
		void AddState(const WideString& stateName, UiObjectState* state);

		typedef std::map<WideString, UiObjectState*> StateMap;
		StateMap m_States;

		WideString m_CurrentState;

		virtual void OnSizeChanged();
	};
}

#endif // __WmeUiControl_H__