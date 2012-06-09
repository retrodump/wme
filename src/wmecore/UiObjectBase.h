// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObjectBase_H__
#define __WmeUiObjectBase_H__


#include "InteractiveObject.h"


namespace Wme
{
	class Canvas2D;
	class SceneNode2D;
	class UiAnchor;

	class WmeDllExport UiObjectBase : public InteractiveObject
	{
	public:

		class WmeDllExport Listener 
		{
		public:
			Listener() {}
			virtual ~Listener() {}

			virtual void OnGeometryChanged() {}
			virtual void OnDestroy() {}
		};

		enum AnchorType
		{
			ANCHOR_NONE,
			ANCHOR_LEFT,
			ANCHOR_RIGHT,
			ANCHOR_TOP,
			ANCHOR_BOTTOM
		};


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

		float GetLeft() const;
		void SetLeft(float left);

		float GetRight() const;
		void SetRight(float right);

		float GetTop() const;
		void SetTop(float top);

		float GetBottom() const;
		void SetBottom(float bottom);

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

		void AddListener(Listener* listener);
		void RemoveListener(Listener* listener);

		bool HasChild(UiObjectBase* object) const;
		bool IsSibling(UiObjectBase* object) const;
		bool IsParent(UiObjectBase* object) const;

		bool SetAnchor(AnchorType type, UiObjectBase* target, float margin = 0.0f);
		bool GetAnchor(AnchorType type, UiObjectBase*& target, float& margin) const;
		bool HasAnchor(AnchorType type) const;

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

		void InvokeGeometryChanged() const;

		typedef std::vector<Listener*> ListenerList;
		ListenerList m_Listeners;

		UiAnchor* m_LeftAnchor;
		UiAnchor* m_RightAnchor;
		UiAnchor* m_TopAnchor;
		UiAnchor* m_BottomAnchor;
	};
}

#endif // __WmeUiObjectBase_H__