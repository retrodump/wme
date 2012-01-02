// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Transform2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Transform2D::Transform2D()
{
	m_Matrix = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
Transform2D::Transform2D(const Ogre::Matrix4& matrix)
{
	m_Matrix = matrix;
}

//////////////////////////////////////////////////////////////////////////
Transform2D::~Transform2D()
{
}

//////////////////////////////////////////////////////////////////////////
bool Transform2D::operator==(const Transform2D& trans) const
{
	return m_Matrix == trans.m_Matrix;
}

//////////////////////////////////////////////////////////////////////////
bool Transform2D::operator!=(const Transform2D& trans) const
{
	return m_Matrix != trans.m_Matrix;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::operator*=(const Transform2D& trans)
{
	m_Matrix = m_Matrix * trans.m_Matrix;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D Transform2D::operator*(const Transform2D& trans) const
{
	Transform2D t = trans;
	return t;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::operator=(const Transform2D& trans)
{
	if (this == &trans) return *this;
	m_Matrix = trans.m_Matrix;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Translate(float x, float y)
{
	Ogre::Matrix4 matTrans;
	matTrans.makeTrans(x, y, 0.0f);

	m_Matrix = m_Matrix * matTrans;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Rotate(float degrees)
{
	Ogre::Matrix4 matRotation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(degrees)), Ogre::Vector3::UNIT_Z));
	m_Matrix = m_Matrix * matRotation;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Scale(float x, float y)
{
	Ogre::Matrix4 matScale = Ogre::Matrix4::IDENTITY;
	matScale.setScale(Ogre::Vector3(x, y, 0.0f));
	m_Matrix = m_Matrix * matScale;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Reset()
{
	m_Matrix = Ogre::Matrix4::IDENTITY;

	return *this;
}


} // namespace Wme
