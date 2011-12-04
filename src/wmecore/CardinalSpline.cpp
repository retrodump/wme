// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CardinalSpline.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
CardinalSpline::CardinalSpline()
{
	// Set up Matrix
	// Hermite polynomial
	m_Coeffs[0][0] = 2;
	m_Coeffs[0][1] = -2;
	m_Coeffs[0][2] = 1;
	m_Coeffs[0][3] = 1;
	m_Coeffs[1][0] = -3;
	m_Coeffs[1][1] = 3;
	m_Coeffs[1][2] = -2;
	m_Coeffs[1][3] = -1;
	m_Coeffs[2][0] = 0;
	m_Coeffs[2][1] = 0;
	m_Coeffs[2][2] = 1;
	m_Coeffs[2][3] = 0;
	m_Coeffs[3][0] = 1;
	m_Coeffs[3][1] = 0;
	m_Coeffs[3][2] = 0;
	m_Coeffs[3][3] = 0;

	m_Tension = 0.0f;

	m_AutoCalc = true;
}

//////////////////////////////////////////////////////////////////////////
CardinalSpline::~CardinalSpline()
{
}

//////////////////////////////////////////////////////////////////////////
void CardinalSpline::AddPoint(const Ogre::Vector3& p)
{
	m_Points.push_back(p);
	if (m_AutoCalc)
	{
		RecalcTangents();
	}
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 CardinalSpline::Interpolate(Ogre::Real t) const
{
	// Currently assumes points are evenly spaced, will cause velocity
	// change where this is not the case
	// TODO: base on arclength?


	// Work out which segment this is in
	Ogre::Real fSeg = t * (m_Points.size() - 1);
	unsigned int segIdx = (unsigned int)fSeg;
	// Apportion t 
	t = fSeg - segIdx;

	return Interpolate(segIdx, t);

}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 CardinalSpline::Interpolate(unsigned int fromIndex, Ogre::Real t) const
{
	// Bounds check
	assert (fromIndex < m_Points.size() &&
		"fromIndex out of bounds");

	if ((fromIndex + 1) == m_Points.size())
	{
		// Duff request, cannot blend to nothing
		// Just return source
		return m_Points[fromIndex];

	}

	// Fast special cases
	if (t == 0.0f)
	{
		return m_Points[fromIndex];
	}
	else if(t == 1.0f)
	{
		return m_Points[fromIndex + 1];
	}

	// Ogre::Real interpolation
	// Form a vector of powers of t
	Ogre::Real t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	Ogre::Vector4 powers(t3, t2, t, 1);


	// Algorithm is ret = powers * mCoeffs * Matrix4(point1, point2, tangent1, tangent2)
	const Ogre::Vector3& point1 = m_Points[fromIndex];
	const Ogre::Vector3& point2 = m_Points[fromIndex+1];
	const Ogre::Vector3& tan1 = m_Tangents[fromIndex];
	const Ogre::Vector3& tan2 = m_Tangents[fromIndex+1];
	Ogre::Matrix4 pt;

	pt[0][0] = point1.x;
	pt[0][1] = point1.y;
	pt[0][2] = point1.z;
	pt[0][3] = 1.0f;
	pt[1][0] = point2.x;
	pt[1][1] = point2.y;
	pt[1][2] = point2.z;
	pt[1][3] = 1.0f;
	pt[2][0] = tan1.x;
	pt[2][1] = tan1.y;
	pt[2][2] = tan1.z;
	pt[2][3] = 1.0f;
	pt[3][0] = tan2.x;
	pt[3][1] = tan2.y;
	pt[3][2] = tan2.z;
	pt[3][3] = 1.0f;

	Ogre::Vector4 ret = powers * m_Coeffs * pt;


	return Ogre::Vector3(ret.x, ret.y, ret.z);
}


//////////////////////////////////////////////////////////////////////////
void CardinalSpline::RecalcTangents()
{
	// Cardinal spline approach
	// 
	// tangent[i] = (1.0 - tension) * 0.5 * (point[i+1] - point[i-1])
	//
	// Assume endpoint tangents are parallel with line with neighbour

	size_t i, numPoints;
	bool isClosed;

	numPoints = m_Points.size();
	if (numPoints < 2)
	{
		// Can't do anything yet
		return;
	}

	// Closed or open?
	if (m_Points[0] == m_Points[numPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}

	m_Tangents.resize(numPoints);



	for(i = 0; i < numPoints; ++i)
	{
		if (i ==0)
		{
			// Special case start
			if (isClosed)
			{
				// Use numPoints-2 since numPoints-1 is the last point and == [0]
				m_Tangents[i] = (1.0f - m_Tension) * 0.5f * (m_Points[1] - m_Points[numPoints-2]);
			}
			else
			{
				m_Tangents[i] = (1.0f - m_Tension) * 0.5f * (m_Points[1] - m_Points[0]);
			}
		}
		else if (i == numPoints-1)
		{
			// Special case end
			if (isClosed)
			{
				// Use same tangent as already calculated for [0]
				m_Tangents[i] = m_Tangents[0];
			}
			else
			{
				m_Tangents[i] = (1.0f - m_Tension) * 0.5f * (m_Points[i] - m_Points[i-1]);
			}
		}
		else
		{
			m_Tangents[i] = (1.0f - m_Tension) * 0.5f * (m_Points[i+1] - m_Points[i-1]);
		}

	}

}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& CardinalSpline::GetPoint(unsigned short index) const
{
	assert (index < m_Points.size() && "Point index is out of bounds!!");

	return m_Points[index];
}

//////////////////////////////////////////////////////////////////////////
unsigned short CardinalSpline::GetNumPoints() const
{
	return (unsigned short)m_Points.size();
}

//////////////////////////////////////////////////////////////////////////
void CardinalSpline::Clear()
{
	m_Points.clear();
	m_Tangents.clear();
}

//////////////////////////////////////////////////////////////////////////
void CardinalSpline::UpdatePoint(unsigned short index, const Ogre::Vector3& value)
{
	assert (index < m_Points.size() && "Point index is out of bounds!!");

	m_Points[index] = value;
	if (m_AutoCalc)
	{
		RecalcTangents();
	}
}

//////////////////////////////////////////////////////////////////////////
void CardinalSpline::SetAutoCalculate(bool autoCalc)
{
	m_AutoCalc = autoCalc;
}


} // namespace Wme
