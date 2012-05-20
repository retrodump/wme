// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObjectBase_H__
#define __WmeUiObjectBase_H__


#include "InteractiveObject.h"


namespace Wme
{
	class Canvas2D;
	class SceneNode2D;

	class WmeDllExport UiObjectBase : public InteractiveObject
	{
	public:
		UiObjectBase(Canvas2D* canvas);
		virtual ~UiObjectBase();

		typedef std::list<UiObjectBase*> UiObjectList;

		Canvas2D* GetCanvas() const { return m_Canvas; }
		SceneNode2D* GetSceneNode() const { return m_SceneNode; }
		SceneNode2D* GetChildrenNode() const { return m_ChildrenNode; }
		
		UiObjectBase* GetParent() const { return m_Parent; }
		void SetParent(UiObjectBase* parent);

		bool GetClipChildren() const { return m_ClipChildren; }
		void SetClipChildren(bool clipChildren);

		float GetPosX() const;
		void SetPosX(float posX);

		float GetPosY() const;
		void SetPosY(float posY);

		float GetWidth() const { return m_Width; }
		void SetWidth(float width);

		float GetHeight() const { return m_Height; }
		void SetHeight(float height);

		bool IsVisible() const;
		void SetVisible(bool visible);

		bool IsEnabled() const;
		void SetEnabled(bool enabled);

		void AddChild(UiObjectBase* child);
		void RemoveChild(UiObjectBase* child);

		void MoveChildAfter(UiObjectBase* child, UiObjectBase* pos);
		void MoveChildBefore(UiObjectBase* child, UiObjectBase* pos);

		size_t GetNumChildren() const { return m_Children.size(); }
		UiObjectBase* GetChild(int index);
		UiObjectBase* GetChild(const WideString& name);


	protected:
		Canvas2D* m_Canvas;
		SceneNode2D* m_SceneNode;
		SceneNode2D* m_ElementsNode;
		SceneNode2D* m_ChildrenNode;

		bool m_Enabled;

		float m_Width;
		float m_Height;

		UiObjectBase* m_Parent;
		UiObjectList m_Children;

		bool m_ClipChildren;

		void UpdateChildrenZOrder();
		void UpdateClippingRect();

		virtual void OnSizeChanged() {}
		virtual void OnEnabledChanged() {}
	};
}

#endif // __WmeUiObjectBase_H__