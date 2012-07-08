// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiInteractiveArea.h"
#include "InteractiveElement2D.h"
#include "UiControl.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiInteractiveArea::UiInteractiveArea(Canvas2D* canvas) : UiElement(canvas)
{
	m_Area = NULL;
}

//////////////////////////////////////////////////////////////////////////
UiInteractiveArea::~UiInteractiveArea()
{
	SAFE_DELETE(m_Area);
}

//////////////////////////////////////////////////////////////////////////
void UiInteractiveArea::OnSizeChanged()
{
	if (m_Area)
	{
		m_Area->SetSize(GetWidth(), GetHeight());
	}
}

//////////////////////////////////////////////////////////////////////////
void UiInteractiveArea::OnParentChanged()
{
	if (!m_Area)
	{
		m_Area = new InteractiveElement2D();
		m_ElementsNode->AttachElement(m_Area);
	}
	m_Area->SetOwner(GetParentControl());
}


} // namespace Wme
