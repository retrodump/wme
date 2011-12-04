// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRegion_H__
#define __WmeRegion_H__


#include "Object.h"


namespace Wme
{
	class WmeDllExport Region : public Object
	{
	public:
		Region();
		~Region();

		void ClearPoints();
		void AddPoint(float x, float y);

		bool IsPointInRegion(float x, float y);

		typedef std::vector<Ogre::Vector2> PointVector;
		const PointVector& GetPoints() const { return m_Points; }

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

	private:
		
		PointVector m_Points;

		void UpdateBoundingBox();
		Ogre::Rectangle m_BoundingBox;
	};
}

#endif // __WmeRegion_H__