// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Canvas2D.h"
#include "Transform2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Canvas2D::Canvas2D()
{
}

//////////////////////////////////////////////////////////////////////////
Canvas2D::~Canvas2D()
{
}

//////////////////////////////////////////////////////////////////////////
void Canvas2D::SetTransform(const Transform2D& transform)
{
	m_Transform = transform;
}

//////////////////////////////////////////////////////////////////////////
void Canvas2D::ResetTransform()
{
	m_Transform.Reset();
}

//////////////////////////////////////////////////////////////////////////
const Transform2D& Canvas2D::GetTransform() const
{
	return m_Transform;
}


} // namespace Wme