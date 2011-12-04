// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ActiveSpot.h"
#include "Viewport.h"
#include "InteractiveObject.h"
#include "SpriteSubFrame.h"
#include "Stage.h"
#include "Camera.h"
#include "Region.h"
#include "MousePickParams.h"
#include "RttView.h"
#include "ViewportLayout.h"
#include "StringUtil.h" // temp
#include "UiButton.h" // temp


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ActiveSpot::ActiveSpot()
{
	m_Object = NULL;
	m_Viewport = NULL;
	m_OffsetX = m_OffsetY = 0;
}

//////////////////////////////////////////////////////////////////////////
ActiveSpot::ActiveSpot(Viewport* viewport, InteractiveObject* object)
{
	m_Object = object;
	m_Viewport = viewport;
	m_OffsetX = viewport->GetScrollOffsetX();
	m_OffsetY = viewport->GetScrollOffsetY();
}

//////////////////////////////////////////////////////////////////////////
ActiveSpot::~ActiveSpot()
{
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ActiveSpotSprite::ActiveSpotSprite(Viewport* viewport, int targetX, int targetY, const SpriteSubFrame* subFrame, const SpritePlacement& placement, const SpriteDrawingParams& params) : ActiveSpot(viewport, params.Parent)
{
	m_TargetX = targetX;
	m_TargetY = targetY;
	m_SubFrame = subFrame;
	m_Placement = placement;
	m_DrawingParams = params;

	float minX, minY, maxX, maxY;

	minX = maxX = placement.Vertices[0].x;
	minY = maxY = placement.Vertices[0].y;

	for (int i = 0; i < 4; i++)
	{
		minX = std::min(minX, placement.Vertices[i].x);
		minY = std::min(minY, placement.Vertices[i].y);

		maxX = std::max(maxX, placement.Vertices[i].x);
		maxY = std::max(maxY, placement.Vertices[i].y);
	}

	m_BoundingBox.x = minX;
	m_BoundingBox.y = minY;
	m_BoundingBox.width = maxX - minX;
	m_BoundingBox.height = maxY - minY;

	m_BoundingBox = viewport->GetClippingRect().GetIntersection(m_BoundingBox);
}

//////////////////////////////////////////////////////////////////////////
ActiveSpotSprite::~ActiveSpotSprite()
{

}

//////////////////////////////////////////////////////////////////////////
bool ActiveSpotSprite::QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result)
{
	int origX = x;
	int origY = y;

	if (!m_DrawingParams.AbsolutePos)
	{
		x += m_OffsetX;
		y += m_OffsetY;
	}


	if (x >= m_BoundingBox.GetLeft() && x <= m_BoundingBox.GetRight() && y >= m_BoundingBox.GetTop() && y <= m_BoundingBox.GetBottom())
	{
		Ogre::Vector3 relPos = Ogre::Vector3((float)origX, (float)origY, 0.0f);

		// if the sprite is rotated, inverse-rotate cursor position around the rotation center
		if (m_Placement.Rotation != 0)
		{
			Ogre::Matrix4 matTrans1 = Ogre::Matrix4::IDENTITY;
			matTrans1.makeTrans(-m_Placement.CenterPoint.x, -m_Placement.CenterPoint.y, 0.0);

			Ogre::Matrix4 matTrans2;
			matTrans2.makeTrans(m_Placement.CenterPoint.x, m_Placement.CenterPoint.y, 0.0);

			Ogre::Matrix4 matRotation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-m_Placement.Rotation)), Ogre::Vector3::UNIT_Z));

			Ogre::Matrix4 matFinal = matTrans2 * matRotation * matTrans1;
			relPos = matFinal * relPos;
		}

		if (!m_DrawingParams.AbsolutePos)
		{
			relPos.x += m_OffsetX;
			relPos.y += m_OffsetY;
		}

		// position, in pixels, within the sprite image
		relPos.x = (relPos.x - m_TargetX) / m_DrawingParams.Scale.x;
		relPos.y = (relPos.y - m_TargetY) / m_DrawingParams.Scale.y;

		if (m_DrawingParams.MirrorY) relPos.x = m_SubFrame->GetWidth() - relPos.x;
		if (m_DrawingParams.MirrorX) relPos.y = m_SubFrame->GetHeight() - relPos.y;


		if (!m_SubFrame->IsTransparentAt((int)relPos.x, (int)relPos.y))
		{
			result.Viewport = m_Viewport;
			result.Object = m_Object;
			return true;
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ActiveSpotGeometry::ActiveSpotGeometry(Viewport* viewport, Stage* stage, Camera* camera, Ogre::uint8 renderQueueId) : ActiveSpot(viewport, NULL)
{
	m_Stage = stage;
	m_Camera = camera;
	m_RenderQueueId = renderQueueId;
}

//////////////////////////////////////////////////////////////////////////
ActiveSpotGeometry::~ActiveSpotGeometry()
{

}

//////////////////////////////////////////////////////////////////////////
bool ActiveSpotGeometry::QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result)
{
	Ogre::Ray mouseray = m_Camera->GetOgreCamera()->getCameraToViewportRay(relX, relY);

	MousePickParams pickParams;
	pickParams.Point = mouseray.getPoint(0);
	pickParams.Normal = mouseray.getDirection();
	pickParams.RenderQueueId = m_RenderQueueId;
	pickParams.QueryUV = true;

	static UiButton dummy(NULL);

	bool ok = m_Stage->RaycastFromPoint(pickParams);
	if (ok)
	{
		if (!pickParams.ResultEntity->getUserObjectBindings().getUserAny("pickable").isEmpty())
		{
			PickableObject pickable = Ogre::any_cast<PickableObject>(pickParams.ResultEntity->getUserObjectBindings().getUserAny("pickable"));
			if (pickable.GetType() == PickableObject::PICKABLE_RTT_VIEW)
			{
				RttView* view = static_cast<RttView*>(pickable.GetObject());
				view->InjectMousePos(pickParams.ResultUV.x, pickParams.ResultUV.y);
				
				int viewX, viewY;
				view->GetMousePos(viewX, viewY);
				
				ViewportLayout* layout = view->GetViewportLayout();
				if (layout) return layout->GetObjectAt(viewX, viewY, result);
			}
		}


		WideString name = StringUtil::Utf8ToWide(pickParams.ResultEntity->getName());
		name += L" UV: " + StringUtil::ToString(pickParams.ResultUV.x) + L", " + StringUtil::ToString(pickParams.ResultUV.y);
		name += L" Mat: " + StringUtil::Utf8ToWide(pickParams.ResultSubEntity->getMaterialName());

		dummy.SetName(name);

		result.Object = &dummy;
		result.Viewport = m_Viewport;
		result.HitPoint = pickParams.ResultPoint;
		result.HitUV = pickParams.ResultUV;

		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ActiveSpotRegion::ActiveSpotRegion(Viewport* viewport, InteractiveObject* object, Region* region, bool absolutePos) : ActiveSpot(viewport, object)
{
	m_Region = region;
	m_AbsolutePos = absolutePos;
}

//////////////////////////////////////////////////////////////////////////
ActiveSpotRegion::~ActiveSpotRegion()
{

}

//////////////////////////////////////////////////////////////////////////
bool ActiveSpotRegion::QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result)
{
	if (!m_AbsolutePos)
	{
		x += m_OffsetX;
		y += m_OffsetY;
	}

	if (m_Region && m_Region->IsPointInRegion((float)x, (float)y))
	{
		result.Viewport = m_Viewport;
		result.Object = m_Object;
		return true;
	}
	else return false;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ActiveSpotRectangle::ActiveSpotRectangle(Viewport* viewport, InteractiveObject* object, const Rect& rect, bool absolutePos) : ActiveSpot(viewport, object)
{	
	m_AbsolutePos = absolutePos;
	m_Rect = viewport->GetClippingRect().GetIntersection(rect);
}

//////////////////////////////////////////////////////////////////////////
ActiveSpotRectangle::~ActiveSpotRectangle()
{

}

//////////////////////////////////////////////////////////////////////////
bool ActiveSpotRectangle::QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result)
{
	if (!m_AbsolutePos)
	{
		x += m_OffsetX;
		y += m_OffsetY;
	}

	if (x >= m_Rect.x && x <= m_Rect.x + m_Rect.width && y >= m_Rect.y && y <= m_Rect.y + m_Rect.height)
	{
		result.Viewport = m_Viewport;
		result.Object = m_Object;
		return true;
	}
	else return false;
}


} // namespace Wme
