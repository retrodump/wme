// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Region.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Region::Region()
{
}

//////////////////////////////////////////////////////////////////////////
Region::~Region()
{
}

//////////////////////////////////////////////////////////////////////////
void Region::ClearPoints()
{
	m_Points.clear();
	UpdateBoundingBox();
}

//////////////////////////////////////////////////////////////////////////
void Region::AddPoint(float x, float y)
{
	m_Points.push_back(Ogre::Vector2(x, y));
	UpdateBoundingBox();
}

//////////////////////////////////////////////////////////////////////////
bool Region::IsPointInRegion(float x, float y)
{
	// http://en.wikipedia.org/wiki/Point_in_polygon

	if (m_Points.size() < 3) return false;
	if (!m_BoundingBox.inside(x, y)) return false;

	int numHits = 0;

	float intersection;
	Ogre::Vector2 p, p1, p2;

	p = Ogre::Vector2(x, y);
	p1 = m_Points[0];

	for (int i = 1; i <= (int)m_Points.size(); i++)
	{
		p2 = m_Points[i % m_Points.size()];

		if (p.y > std::min(p1.y, p2.y))
		{
			if (p.y <= std::max(p1.y, p2.y))
			{
				if (p.x <= std::max(p1.x, p2.x))
				{
					if (p1.y != p2.y)
					{
						intersection = (p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
						if (p1.x == p2.x || p.x <= intersection)
							numHits++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (numHits % 2 == 0)
		return false;
	else
		return true;

}

//////////////////////////////////////////////////////////////////////////
void Region::UpdateBoundingBox()
{
	m_BoundingBox.left = m_BoundingBox.right = m_BoundingBox.bottom = m_BoundingBox.top = 0;
	if (m_Points.size() == 0) return;

	bool first = true;
	foreach (Ogre::Vector2 point, m_Points)
	{
		if (first)
		{
			m_BoundingBox.left = m_BoundingBox.right = point.x;
			m_BoundingBox.top = m_BoundingBox.bottom = point.y;
			first = false;
		}
		else
		{
			m_BoundingBox.left = std::min(m_BoundingBox.left, point.x);
			m_BoundingBox.right = std::max(m_BoundingBox.right, point.x);

			m_BoundingBox.top = std::min(m_BoundingBox.top, point.y);
			m_BoundingBox.bottom = std::max(m_BoundingBox.bottom, point.y);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool Region::LoadFromXml(TiXmlElement* rootNode)
{
	m_Points.clear();

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Point")
		{
			float x = XmlUtil::AttrToFloat(elem, "X");
			float y = XmlUtil::AttrToFloat(elem, "Y");

			m_Points.push_back(Ogre::Vector2(x, y));
		}
	}
	UpdateBoundingBox();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Region::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	foreach (Ogre::Vector2 point, m_Points)
	{
		elem = XmlUtil::AddElem("Point", rootNode);
		XmlUtil::SetAttr(elem, "X", point.x);
		XmlUtil::SetAttr(elem, "Y", point.y);
	}
	return true;
}


} // namespace Wme
