// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MousePickParams.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
MousePickParams::MousePickParams()
{
	Point = Ogre::Vector3::ZERO;
	Normal = Ogre::Vector3::ZERO;
	RenderQueueId = -1;
	QueryUV = false;

	ResultPoint = Ogre::Vector3::ZERO;
	ResultUV = Ogre::Vector2::ZERO;
	ResultEntity = NULL;
	ResultSubEntity = NULL;
}

//////////////////////////////////////////////////////////////////////////
MousePickParams::~MousePickParams()
{
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
MousePickResult::MousePickResult()
{
	Object = NULL;
	Viewport = NULL;
	HitPoint = Ogre::Vector3::ZERO;
	HitUV = Ogre::Vector2::ZERO;
}

//////////////////////////////////////////////////////////////////////////
MousePickResult::~MousePickResult()
{
}


} // namespace Wme