// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiBorder.h"
#include "ResizableImage.h"
#include "ResizableElement2D.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiBorder::UiBorder(Canvas2D* canvas) : UiElement(canvas)
{
	m_Image.SetValue(NULL);
	m_Elem = NULL;
}

//////////////////////////////////////////////////////////////////////////
UiBorder::~UiBorder()
{
	m_Image.DeleteAllValues();
}

//////////////////////////////////////////////////////////////////////////
void UiBorder::SetImage(ResizableImage* image, const WideString& stateName)
{
	if (!m_Elem)
	{
		m_Elem = new ResizableElement2D();
		m_ElementsNode->AttachElement(m_Elem);
	}

	if (stateName == m_CurrentState)
	{
		m_Elem->SetImage(image);
	}

	m_Image.DeleteValue(stateName);
	m_Image.SetValue(image, stateName);
}

//////////////////////////////////////////////////////////////////////////
void UiBorder::SetImage(const WideString& fileName, const WideString& stateName)
{
	ResizableImage* image = new ResizableImage();
	image->LoadFromFile(fileName);

	SetImage(image, stateName);
}

//////////////////////////////////////////////////////////////////////////
void UiBorder::OnSizeChanged()
{
	if (m_Elem)
	{
		m_Elem->SetSize(GetWidth(), GetHeight());
	}
}

//////////////////////////////////////////////////////////////////////////
void UiBorder::OnStateChanged()
{
	m_Elem->SetImage(m_Image.GetValue(m_CurrentState));
}


} // namespace Wme
