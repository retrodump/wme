// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiWindowOld.h"
#include "UiObjectStyleOld.h"
#include "Viewport.h"
#include "ActiveSpot.h"
#include "ResizableImageOld.h"
#include "Sprite.h"
#include "ViewportLayout.h"
#include "View.h"
#include "Camera.h"
#include "GuiStage.h"
#include "RenderModifier.h"
#include "ElementCollection.h"
#include "ContentManager.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiWindowOld::UiWindowOld(GuiStage* parentStage) : UiObjectOld(parentStage)
{
	m_NormalStyle = NULL;
	m_DisabledStyle = NULL;
	m_InactiveStyle = NULL;

	m_ClipContent = true;
	m_ClickThrough = false;

	m_Clipper = m_UnClipper = NULL;

	m_IsDragging = false;
	m_DragViewport = NULL;
	m_DragStartX = m_DragStartY = 0;
}

//////////////////////////////////////////////////////////////////////////
UiWindowOld::~UiWindowOld()
{
	SAFE_DELETE(m_NormalStyle);
	SAFE_DELETE(m_DisabledStyle);
	SAFE_DELETE(m_InactiveStyle);

	SAFE_DELETE(m_Clipper);
	SAFE_DELETE(m_UnClipper);
}

//////////////////////////////////////////////////////////////////////////
void UiWindowOld::Create()
{
	m_NormalStyle = new UiObjectStyleOld(this);
	m_DisabledStyle = new UiObjectStyleOld(this);
	m_InactiveStyle = new UiObjectStyleOld(this);

	m_Clipper = new RenderModifier;
	m_UnClipper = new RenderModifier;

	UiObjectOld::Create();
}

//////////////////////////////////////////////////////////////////////////
void UiWindowOld::Display(ElementCollection* elementCol, const SpriteDrawingParams& params)
{
	if (!m_Visible) return;

	Rect windowRect = GetAbsoluteRect();

	Sprite* sprite = GetSprite();
	ResizableImageOld* image = GetImage();

	// start clipping
	if (m_ClipContent)
	{
		m_Clipper->SetClipping(true, params.Viewport, params.Viewport->GetClippingRect().GetIntersection(windowRect));
		elementCol->AddElement(m_Clipper);
	}

	// render content
	if (image) image->Display(elementCol, params);
	if (sprite)
	{
		SpriteDrawingParams newParams = params;
		if (m_PixelPerfect && !m_ClickThrough && !m_Disabled)
			newParams.Parent = this;
		else
			newParams.Parent = NULL;

		sprite->Display(elementCol, GetAbsoluteX(), GetAbsoluteY(), newParams);
	}

	// no sprite or not pixel perfect -> add rectangular active spot
	if (!m_Disabled && !m_ClickThrough && (!m_PixelPerfect || !sprite))
	{
		params.Viewport->AddActiveSpot(new ActiveSpotRectangle(params.Viewport, this, windowRect, params.AbsolutePos));
	}

	// render children
	UiObjectOld::Display(elementCol, params);


	// stop clipping
	if (m_ClipContent)
	{
		m_UnClipper->SetClipping(false, params.Viewport);
		elementCol->AddElement(m_UnClipper);
	}
}

//////////////////////////////////////////////////////////////////////////
void UiWindowOld::Update()
{
	if (!m_Visible) return;

	Sprite* sprite = GetSprite();
	ResizableImageOld* image = GetImage();

	if (m_IsDragging)
	{
		int x, y;
		m_DragViewport->GetMousePos(x, y);

		m_PosX += x - m_DragStartX;
		m_PosY += y - m_DragStartY;

		m_DragStartX = x;
		m_DragStartY = y;
	}


	if (sprite) sprite->Update();

	if (image)
	{
		image->SetPos(GetAbsoluteX(), GetAbsoluteY());
		image->SetSize(GetWidth(), GetHeight());
	}

	UiObjectOld::Update();
}

//////////////////////////////////////////////////////////////////////////
int UiWindowOld::GetRealWidth() const
{
	ResizableImageOld* image = GetImage();
	if (image) return image->GetCorrectedWidth();
	else return m_Width;
}

//////////////////////////////////////////////////////////////////////////
int UiWindowOld::GetRealHeight() const
{
	ResizableImageOld* image = GetImage();
	if (image) return image->GetCorrectedHeight();
	else return m_Height;
}

//////////////////////////////////////////////////////////////////////////
UiObjectStyleOld* UiWindowOld::GetCurrentStyle() const
{
	if (m_Disabled) return m_DisabledStyle;

	return m_NormalStyle;
}

//////////////////////////////////////////////////////////////////////////
UiObjectStyleOld* UiWindowOld::GetStyle(WindowState state) const
{
	switch (state)
	{
	case STATE_DISABLED:
		return m_DisabledStyle;
	case STATE_INACTIVE:
		return m_InactiveStyle;
	default:
		return m_NormalStyle;
	}
}


//////////////////////////////////////////////////////////////////////////
ResizableImageOld* UiWindowOld::GetImage() const
{
	ResizableImageOld* ret;

	UiObjectStyleOld* style = GetCurrentStyle();
	ret = style->GetImage();
	if (!ret) ret = m_NormalStyle->GetImage();
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Sprite* UiWindowOld::GetSprite() const
{
	Sprite* ret;

	UiObjectStyleOld* style = GetCurrentStyle();
	ret = style->GetSprite();
	if (!ret) ret = m_NormalStyle->GetSprite();
	return ret;
}


//////////////////////////////////////////////////////////////////////////
bool UiWindowOld::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	event.TranslateToViewport(viewport);

	int x = event.GetPosX() - GetAbsoluteX();
	int y = event.GetPosY() - GetAbsoluteY();

	if (event.GetButton() == MouseEvent::BUTTON_LEFT)
	{
		if (event.GetType() == MouseEvent::EVENT_PRESS)
		{
			Game::GetInstance()->GetContentMgr()->CaptureMouse(this, viewport);
			m_IsDragging = true;
			m_DragViewport = viewport;
			m_DragStartX = event.GetPosX();
			m_DragStartY = event.GetPosY();
		}
		else if (event.GetType() == MouseEvent::EVENT_RELEASE)
		{
			Game::GetInstance()->GetContentMgr()->ReleaseMouse(this);
			m_IsDragging = false;
			m_DragViewport = NULL;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool UiWindowOld::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "ClickThrough")
		{
			m_ClickThrough = XmlUtil::TextToBool(elem, false);
		}
		else if (elem->ValueStr() == "ClipContent")
		{
			m_ClipContent = XmlUtil::TextToBool(elem, false);
		}
		// styles
		else if (elem->ValueStr() == "NormalStyle")
		{
			m_NormalStyle->LoadFromXml(elem);
		}
		else if (elem->ValueStr() == "InactiveStyle")
		{
			m_InactiveStyle->LoadFromXml(elem);
		}
		else if (elem->ValueStr() == "DisabledStyle")
		{
			m_DisabledStyle->LoadFromXml(elem);
		}
	}

	return UiObjectOld::LoadFromXml(rootNode);
}

//////////////////////////////////////////////////////////////////////////
bool UiWindowOld::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("ClickThrough", rootNode);
	XmlUtil::SetText(elem, m_ClickThrough);

	elem = XmlUtil::AddElem("ClipContent", rootNode);
	XmlUtil::SetText(elem, m_ClipContent);

	// styles
	elem = XmlUtil::AddElem("NormalStyle", rootNode);
	m_NormalStyle->SaveToXml(elem);

	elem = XmlUtil::AddElem("InactiveStyle", rootNode);
	m_InactiveStyle->SaveToXml(elem);

	elem = XmlUtil::AddElem("DisabledStyle", rootNode);
	m_DisabledStyle->SaveToXml(elem);


	return UiObjectOld::SaveToXml(rootNode);
}


} // namespace Wme
