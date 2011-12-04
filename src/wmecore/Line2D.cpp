// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Line2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Line2D::Line2D()
{
	m_StartPoint = m_EndPoint = Ogre::Vector2::ZERO;
	ComputeNormal();
}

//////////////////////////////////////////////////////////////////////////
Line2D::Line2D(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint)
{
	m_StartPoint = startPoint;
	m_EndPoint = endPoint;
	ComputeNormal();
}

//////////////////////////////////////////////////////////////////////////
Line2D::Line2D(const Line2D& orig)
{
	*this = orig;
}

//////////////////////////////////////////////////////////////////////////
Line2D::~Line2D()
{
}

//////////////////////////////////////////////////////////////////////////
void Line2D::SetPoints(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint)
{
	m_StartPoint = startPoint;
	m_EndPoint = endPoint;
	ComputeNormal();
}

//////////////////////////////////////////////////////////////////////////
void Line2D::ComputeNormal()
{
	m_Normal = (m_EndPoint - m_StartPoint);
	m_Normal.normalise();

	// rotate 90 degrees
	Ogre::Real origY = m_Normal.y;
	m_Normal.y = -m_Normal.x;
	m_Normal.x = origY;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real Line2D::GetLength() const
{
	Ogre::Real xDist = m_EndPoint.x - m_StartPoint.x;
	Ogre::Real yDist = m_EndPoint.y - m_StartPoint.y;

	return Ogre::Math::Sqrt(xDist * xDist + yDist * yDist);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real Line2D::GetSquaredLength() const
{
	Ogre::Real xDist = m_EndPoint.x - m_StartPoint.x;
	Ogre::Real yDist = m_EndPoint.y - m_StartPoint.y;

	return xDist * xDist + yDist * yDist;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real Line2D::GetSignedDistance(const Ogre::Vector2& point) const
{
	Ogre::Vector2 testVector(point - m_StartPoint);

	return testVector.dotProduct(m_Normal);
}

//////////////////////////////////////////////////////////////////////////
Line2D::PointClassification Line2D::ClassifyPoint(const Ogre::Vector2& point, Ogre::Real epsilon) const
{
	Ogre::Real distance = GetSignedDistance(point);

	if (distance > epsilon)
	{
		return POINT_RIGHT;
	}
	else if (distance < -epsilon)
	{
		 return POINT_LEFT;
	}

	return POINT_ON_LINE;
}

//////////////////////////////////////////////////////////////////////////
Line2D::LineClassification Line2D::Intersects(const Line2D& line, Ogre::Vector2* intersectionPoint) const
{
	Ogre::Real ayCy = m_StartPoint.y - line.GetStartPoint().y;	
	Ogre::Real dxCx = line.GetEndPoint().x - line.GetStartPoint().x;	
	Ogre::Real axCx = m_StartPoint.x - line.GetStartPoint().x;	
	Ogre::Real dyCy = line.GetEndPoint().y - line.GetStartPoint().y;	
	Ogre::Real bxAx = m_EndPoint.x - m_StartPoint.x;	
	Ogre::Real byAy = m_EndPoint.y - m_StartPoint.y;	

	Ogre::Real numerator = (ayCy * dxCx) - (axCx * dyCy);
	Ogre::Real denominator = (bxAx * dyCy) - (byAy * dxCx);

	// if lines do not intersect, return now
	if (!denominator)
	{
		if (!numerator)
		{
			return COLLINEAR;
		}
		return PARALLEL;
	}

	Ogre::Real factorAB = numerator / denominator;
	Ogre::Real factorCD = ((ayCy * bxAx) - (axCx * byAy)) / denominator;

	// if an interection point was provided, fill it in now
	if (intersectionPoint)
	{
		intersectionPoint->x = (m_StartPoint.x + (factorAB * bxAx));
		intersectionPoint->y = (m_StartPoint.y + (factorAB * byAy));
	}

	// now determine the type of intersection
	if ((factorAB >= 0.0f) && (factorAB <= 1.0f) && (factorCD >= 0.0f) && (factorCD <= 1.0f))
	{
		return SEGMENTS_INTERSECT;
	}
	else if ((factorCD >= 0.0f) && (factorCD <= 1.0f))
	{
		return A_BISECTS_B;
	}
	else if ((factorAB >= 0.0f) && (factorAB <= 1.0f))
	{
		return B_BISECTS_A;
	}

	return LINES_INTERSECT;
}



} // namespace Wme
