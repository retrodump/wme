// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiWindow.h"
#include "SceneNode2D.h"
#include "ResizableElement2D.h"

// temp
#include "ResizableImage.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiWindow::UiWindow(Canvas2D* canvas) : UiControl(canvas)
{
	m_BackgroundNode = m_ElementsNode->CreateChildNode();
	m_Background = new ResizableElement2D();

	m_BackgroundNode->AttachElement(m_Background);

	// temp
	m_ResizableTexture = new ResizableImage();
	m_ResizableTexture->SetTexture(L"win.bmp");
	m_ResizableTexture->SetFrameLeftWidth(30);
	m_ResizableTexture->SetFrameRightWidth(30);
	m_ResizableTexture->SetFrameTopHeight(30);
	m_ResizableTexture->SetFrameBottomHeight(30);

	m_ResizableTexture->SaveToFile(L"c:/win.image");


	m_Background->SetImage(m_ResizableTexture);
}

//////////////////////////////////////////////////////////////////////////
UiWindow::~UiWindow()
{
	m_BackgroundNode->RemoveAndDestroyAllChildren(true);
	SAFE_DELETE(m_BackgroundNode);

	SAFE_DELETE(m_ResizableTexture);
}

//////////////////////////////////////////////////////////////////////////
void UiWindow::OnSizeChanged()
{
	m_Background->SetWidth(GetWidth());
	m_Background->SetHeight(GetHeight());
}


} // namespace Wme
