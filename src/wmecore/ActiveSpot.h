// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeActiveSpot_H__
#define __WmeActiveSpot_H__


#include "Rect.h"
#include "Sprite.h"
#include "MousePickParams.h"


namespace Wme
{
	class InteractiveObject;
	class SpriteSubFrame;
	class Stage;
	class Camera;
	class Region;
	class Viewport;

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ActiveSpot
	{
	public:
		ActiveSpot();
		ActiveSpot(Viewport* viewport, InteractiveObject* object);
		virtual ~ActiveSpot();
		virtual bool QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result) = 0;
	protected:
		InteractiveObject* m_Object;
		int m_OffsetX;
		int m_OffsetY;
		Viewport* m_Viewport;
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ActiveSpotSprite : public ActiveSpot
	{
	public:
		ActiveSpotSprite(Viewport* viewport, int targetX, int targetY, const SpriteSubFrame* subFrame, const SpritePlacement& placement, const SpriteDrawingParams& params);
		virtual ~ActiveSpotSprite();
		virtual bool QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result);
	private:
		int m_TargetX;
		int m_TargetY;
		const SpriteSubFrame* m_SubFrame;
		SpritePlacement m_Placement;
		SpriteDrawingParams m_DrawingParams;
		Rect m_BoundingBox;
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ActiveSpotGeometry : public ActiveSpot
	{
	public:
		ActiveSpotGeometry(Viewport* viewport, Stage* stage, Camera* camera, Ogre::uint8 renderQueueId);
		virtual ~ActiveSpotGeometry();
		virtual bool QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result);
	private:
		Ogre::uint8 m_RenderQueueId;
		Stage* m_Stage;
		Camera* m_Camera;
	
	};

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ActiveSpotRegion : public ActiveSpot
	{
	public:
		ActiveSpotRegion(Viewport* viewport, InteractiveObject* object, Region* region, bool absolutePos);
		virtual ~ActiveSpotRegion();
		virtual bool QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result);
	private:
		Region* m_Region;
		bool m_AbsolutePos;
	};

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ActiveSpotRectangle : public ActiveSpot
	{
	public:
		ActiveSpotRectangle(Viewport* viewport, InteractiveObject* object, const Rect& rect, bool absolutePos);
		virtual ~ActiveSpotRectangle();
		virtual bool QueryObjectAt(int x, int y, float relX, float relY, MousePickResult& result);
	private:
		Rect m_Rect;
		bool m_AbsolutePos;
	};
}


#endif // __WmeActiveSpot_H__
