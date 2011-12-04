// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLine2D_H__
#define __WmeLine2D_H__


namespace Wme
{
	// Portions Copyright (C) Greg Snook, 2000

	class WmeDllExport Line2D
	{
	public:
		Line2D();
		Line2D(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint);
		Line2D(const Line2D& orig);
		~Line2D();

		void SetPoints(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint);

		const Ogre::Vector2& GetStartPoint() const { return m_StartPoint; }
		const Ogre::Vector2& GetEndPoint() const { return m_EndPoint; }
		const Ogre::Vector2& GetNormal() const { return m_Normal; }
		Ogre::Real GetLength() const;
		Ogre::Real GetSquaredLength() const;
		Ogre::Real GetSignedDistance(const Ogre::Vector2& point) const;
		
		/*
		          - end point
				  |
			      |
			  x   |   x
			left  |  right
			      |
				  |
			      - start point
		*/
		enum PointClassification
		{
			POINT_ON_LINE,
			POINT_LEFT,
			POINT_RIGHT
		};		
		PointClassification ClassifyPoint(const Ogre::Vector2& point, Ogre::Real epsilon = 0.0f) const;

		enum LineClassification
		{
			COLLINEAR,			// both lines are parallel and overlap each other
			LINES_INTERSECT,	// lines intersect, but their segments do not
			SEGMENTS_INTERSECT,	// both line segments bisect each other
			A_BISECTS_B,		// line segment B is crossed by line A
			B_BISECTS_A,		// line segment A is crossed by line B
			PARALLEL			// the lines are paralell
		};
		LineClassification Intersects(const Line2D& line, Ogre::Vector2* intersectionPoint = NULL) const;


	private:
		Ogre::Vector2 m_StartPoint;
		Ogre::Vector2 m_EndPoint;
		
		Ogre::Vector2 m_Normal;
		void ComputeNormal();
	};
}


#endif // __WmeLine2D_H__