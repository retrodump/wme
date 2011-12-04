// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObject_H__
#define __WmeUiObject_H__


#include "InteractiveObject.h"
#include "DocumentAwareObject.h"
#include "Rect.h"


namespace Wme
{
	class GuiStage;
	class ElementCollection;
	class SpriteDrawingParams;

	class WmeDllExport UiObject : public InteractiveObject, public DocumentAwareObject
	{
	public:
		typedef UiObject* (*UiObjectActivator)(GuiStage* parentStage);

		UiObject(GuiStage* parentStage);
		virtual ~UiObject();

		typedef std::list<UiObject*> UiObjectList;

		virtual void Display(ElementCollection* elementCol, const SpriteDrawingParams& params);
		virtual void Update();
		virtual void Create();

		UiObject* GetParent() const { return m_Parent; }
		
		size_t GetZOrder() const { return m_ZOrder; }

		void AddChild(UiObject* child);
		void RemoveChild(UiObject* child);

		void MoveChildAfter(UiObject* child, UiObject* pos);
		void MoveChildBefore(UiObject* child, UiObject* pos);

		size_t GetNumChildren() const { return m_Children.size(); }
		UiObject* GetChild(size_t index);
		UiObject* GetChild(const WideString& name);

		void SetPosX(int posX) { m_PosX = posX; }
		int GetPosX() const { return m_PosX; }

		void SetPosY(int posY) { m_PosY = posY; }
		int GetPosY() const { return m_PosY; }

		int GetAbsoluteX() const;
		int GetAbsoluteY() const;

		Rect GetAbsoluteRect() const { return Rect((float)GetAbsoluteX(), (float)GetAbsoluteY(), (float)GetRealWidth(), (float)GetRealHeight()); }

		void SetWidth(int width) { m_Width = width; }
		int GetWidth() const { return m_Width; }

		void SetHeight(int height) { m_Height = height; }
		int GetHeight() const { return m_Height; }

		virtual int GetRealWidth() const { return m_Width; }
		virtual int GetRealHeight() const { return m_Height; }

		void SetPos(int posX, int posY) { m_PosX = posX; m_PosY = posY; }
		void SetSize(int width, int height) { m_Width = width; m_Height = height; }

		GuiStage* GetParentStage() const { return m_ParentStage; }

		bool GetPixelPerfect() const { return m_PixelPerfect; }
		void SetPixelPerfect(bool val) { m_PixelPerfect = val; }

		bool IsDisabled() const { return m_Disabled; }
		void SetDisabled(bool val) { m_Disabled = val; }

		bool IsVisible() const { return m_Visible; }
		void SetVisible(bool val) { m_Visible = val; }

		WideString GetText() const { return m_Text; }
		void SetText(const WideString& val) { m_Text = val; }

		virtual void InvokeEvent(const WideString& eventName) {}

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() = 0;

	protected:
		GuiStage* m_ParentStage;

		UiObject* m_Parent;		
		UiObjectList m_Children;
		size_t m_ZOrder;

		int m_PosX;
		int m_PosY;
		int m_Width;
		int m_Height;

		bool m_PixelPerfect;
		bool m_Disabled;
		bool m_Visible;
		WideString m_Text;

		void UpdateChildrenZOrder();
		void SetZOrder(size_t order) { m_ZOrder = order; }
		void SetParent(UiObject* parent) { m_Parent = parent; }
		void GetOffset(int& offsetX, int& offsetY) const;
	};
}

#endif // __WmeUiObject_H__
