// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "RectElement2D.h"
#include "SceneNode2D.h"
#include "MaterialUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
RectElement2D::RectElement2D()
{
	m_Width = m_Height = 0.0f;
	m_StrokeThickness = 0.0f;
	m_Filled = true;
	m_StrokeColor = Ogre::ColourValue::Black;
	m_FillColor = Ogre::ColourValue::White;

	m_Material = MaterialUtil::GetGeometry2DMat();
}

//////////////////////////////////////////////////////////////////////////
RectElement2D::~RectElement2D()
{
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::AddGeometry()
{
	if (m_Width == 0.0f && m_Height == 0.0f) return;

	int numVerts = 0;
	if (m_Filled) numVerts += 6;
	if (m_StrokeThickness > 0.0f) numVerts += 4 * 6;

	Vertex2D* verts = new Vertex2D[numVerts];
	int vertexOffset = 0;

	if (m_StrokeThickness > 0.0f)
	{
		AddRect(0, 0, m_Width, m_StrokeThickness, m_StrokeColor, verts, vertexOffset);
		AddRect(0, m_Height - m_StrokeThickness, m_Width, m_StrokeThickness, m_StrokeColor, verts, vertexOffset);
		AddRect(0, m_StrokeThickness, m_StrokeThickness, m_Height - 2 * m_StrokeThickness, m_StrokeColor, verts, vertexOffset);
		AddRect(m_Width - m_StrokeThickness, m_StrokeThickness, m_StrokeThickness, m_Height - 2 * m_StrokeThickness, m_StrokeColor, verts, vertexOffset);
	}

	if (m_Filled)
	{
		AddRect(m_StrokeThickness, m_StrokeThickness, m_Width - 2.0f * m_StrokeThickness, m_Height - 2.0f * m_StrokeThickness, m_FillColor, verts, vertexOffset);
	}


	m_ParentNode->AddGeometry(verts, numVerts, m_Material, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	delete [] verts;
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::AddRect(float x, float y, float width, float height, const Ogre::ColourValue& color, Vertex2D* vertBuffer, int& vertexOffset)
{
	Vertex2D* verts = &vertBuffer[vertexOffset];

	// top left
	verts[0].color = color;
	verts[0].pos = Ogre::Vector2(x, y);

	// bottom left
	verts[1].color = color;
	verts[1].pos = Ogre::Vector2(x, y + height);

	// top right
	verts[2].color = color;
	verts[2].pos = Ogre::Vector2(x + width, y);

	// bottom right
	verts[5].color = color;
	verts[5].pos = Ogre::Vector2(x + width, y + height);

	// the other triangle
	verts[3] = verts[2];
	verts[4] = verts[1];


	vertexOffset += 6;
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::SetWidth(float val)
{
	if (val != m_Width)
	{
		m_Width = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::SetHeight(float val)
{
	if (val != m_Height)
	{
		m_Height = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::SetStrokeThickness(float val)
{
	if (val != m_StrokeThickness)
	{
		m_StrokeThickness = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::SetFilled(bool val)
{
	if (val != m_Filled)
	{
		m_Filled = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::SetFillColor(const Ogre::ColourValue& val)
{
	if (val != m_FillColor)
	{
		m_FillColor = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void RectElement2D::SetStrokeColor(const Ogre::ColourValue& val)
{
	if (val != m_StrokeColor)
	{
		m_StrokeColor = val;
		SetDirty();
	}
}


} // namespace Wme
