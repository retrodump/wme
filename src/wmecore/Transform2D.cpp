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
Ogre::Vector2 Transform2D::operator* (const Ogre::Vector2& v) const
{
	Ogre::Vector3 ret = m_Matrix * Ogre::Vector3(v.x, v.y, 0.0f);
	return Ogre::Vector2(ret.x, ret.y);
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Translate(float x, float y)
{
	Ogre::Matrix4 matTrans;
	matTrans.makeTrans(x, y, 0.0f);

	m_Matrix = matTrans * m_Matrix;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Rotate(const Ogre::Degree& angle)
{
	Ogre::Matrix4 matRotation(Ogre::Quaternion(Ogre::Radian(angle), Ogre::Vector3::UNIT_Z));
	m_Matrix = matRotation * m_Matrix;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Scale(float x, float y)
{
	Ogre::Matrix4 matScale = Ogre::Matrix4::IDENTITY;
	matScale.setScale(Ogre::Vector3(x, y, 1.0f));
	m_Matrix = matScale * m_Matrix;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D& Transform2D::Reset()
{
	m_Matrix = Ogre::Matrix4::IDENTITY;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
Transform2D Transform2D::Inverted() const
{
	Transform2D ret;
	ret.SetMatrix(m_Matrix.inverse());

	return ret;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 Transform2D::GetTranslation() const
{
	Ogre::Vector3 trans = m_Matrix.getTrans();
	return Ogre::Vector2(trans.x, trans.y);
}


} // namespace Wme
