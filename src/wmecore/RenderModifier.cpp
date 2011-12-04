// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "RenderModifier.h"
#include "Viewport.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
RenderModifier::RenderModifier()
{
	m_Viewport = NULL;
	m_Clip = false;
	m_Material = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");

	mUseIdentityProjection = true;
	mUseIdentityView = true;
	mPolygonModeOverrideable = false;
}

//////////////////////////////////////////////////////////////////////////
RenderModifier::~RenderModifier()
{
}

//////////////////////////////////////////////////////////////////////////
void RenderModifier::SetClipping(bool clip, Viewport* viewport, const Rect rect)
{
	m_Clip = clip;
	m_Viewport = viewport;
	m_ClippingRect = rect;

	if (m_Clip) m_Viewport->PushClippingRect(m_ClippingRect, false);
	else m_Viewport->PopClippingRect(false);
}

//////////////////////////////////////////////////////////////////////////
// IRenderableProvider
//////////////////////////////////////////////////////////////////////////
void RenderModifier::GetRenderables(IRenderableProvider::RenderableList& renderableList)
{
	renderableList.push_back(this);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Renderable implementation
//////////////////////////////////////////////////////////////////////////
bool RenderModifier::preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rsys)
{
	if (m_Viewport)
	{
		if (m_Clip) m_Viewport->PushClippingRect(m_ClippingRect, true);
		else m_Viewport->PopClippingRect(true);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr& RenderModifier::getMaterial(void) const
{
	return m_Material;
}

//////////////////////////////////////////////////////////////////////////
void RenderModifier::getRenderOperation(Ogre::RenderOperation& op)
{
}

//////////////////////////////////////////////////////////////////////////
void RenderModifier::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::LightList& RenderModifier::getLights(void) const
{
	// N/A
	static Ogre::LightList ll;
	return ll;
}


} // namespace Wme
