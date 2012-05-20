// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Window.h"
#include "SceneNode2D.h"
#include "ResizableElement2D.h"

// temp
#include "SpriteTexture.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Window::Window(Canvas2D* canvas) : UiObjectBase(canvas)
{
	m_BackgroundNode = m_ElementsNode->CreateChildNode();
	m_Background = new ResizableElement2D();

	m_BackgroundNode->AttachElement(m_Background);

	// temp
	m_ResizableTexture = new SpriteTexture();
	m_ResizableTexture->SetTexture(L"win.bmp");

	m_Background->SetTexture(m_ResizableTexture);
	m_Background->SetFrameLeftWidth(30);
	m_Background->SetFrameRightWidth(30);
	m_Background->SetFrameTopHeight(30);
	m_Background->SetFrameBottomHeight(30);
}

//////////////////////////////////////////////////////////////////////////
Window::~Window()
{
	m_BackgroundNode->RemoveAndDestroyAllChildren(true);
	SAFE_DELETE(m_BackgroundNode);

	SAFE_DELETE(m_ResizableTexture);
}

//////////////////////////////////////////////////////////////////////////
void Window::OnSizeChanged()
{
	m_Background->SetWidth(GetWidth());
	m_Background->SetHeight(GetHeight());
}


} // namespace Wme
