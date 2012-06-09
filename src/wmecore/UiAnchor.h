// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiAnchor_H__
#define __WmeUiAnchor_H__


#include "UiObjectBase.h"


namespace Wme
{
	class WmeDllExport UiAnchor : public UiObjectBase::Listener
	{
	public:

		UiAnchor(UiObjectBase* owner, UiObjectBase* target, UiObjectBase::AnchorType type, float margin = 0.0f);
		virtual ~UiAnchor();

		UiObjectBase::AnchorType GetType() const { return m_Type; }
		UiObjectBase* GetTarget() const { return m_Target; }
		float GetMargin() const { return m_Margin; }

		// UiObjectBase::Listener
		void OnGeometryChanged();
		void OnDestroy();

	private:
		UiObjectBase::AnchorType m_Type;
		UiObjectBase* m_Owner;
		UiObjectBase* m_Target;
		float m_Margin;
		bool m_UpdateInProgress;

		void UpdateFromSibling();
		void UpdateFromParent();
	};
}

#endif // __WmeUiAnchor_H__