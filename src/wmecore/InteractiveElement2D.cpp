// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "InteractiveElement2D.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
InteractiveElement2D::InteractiveElement2D()
{
	m_Width = m_Height = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
InteractiveElement2D::~InteractiveElement2D()
{
}

//////////////////////////////////////////////////////////////////////////
void InteractiveElement2D::SetWidth(float val)
{
	if (val != m_Width)
	{
		m_Width = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void InteractiveElement2D::SetHeight(float val)
{
	if (val != m_Height)
	{
		m_Height = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void InteractiveElement2D::SetSize(float width, float height)
{
	if (width != m_Width || height != m_Height)
	{
		m_Width = width;
		m_Height = height;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void InteractiveElement2D::AddGeometry()
{
	// we provide no geometry so we have to set the bounding rect explicitly
	m_ParentNode->SetBoundingRect(0.0f, 0.0f, m_Width, m_Height);
}

//////////////////////////////////////////////////////////////////////////
bool InteractiveElement2D::IsTransparentAt(float x, float y)
{
	return x < 0.0f || x > m_Width || y < 0.0f || y > m_Height;
}


} // namespace Wme
