// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "HybridCamera.h"
#include "Viewport.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
HybridCamera::HybridCamera()
{
}

//////////////////////////////////////////////////////////////////////////
HybridCamera::~HybridCamera()
{
}

//////////////////////////////////////////////////////////////////////////
void HybridCamera::AdjustToViewport(Viewport* viewport)
{
	if (!viewport->GetOgreViewport()) return;

	Ogre::Camera* camera = GetOgreCamera();
	if (!camera) return;

	Ogre::Real originalFrustumLeft, originalFrustumRight, originalFrustumTop, originalFrustumBottom;
	Ogre::Real frustumLeft, frustumRight, frustumTop, frustumBottom;

	camera->resetFrustumExtents();
	camera->getFrustumExtents(originalFrustumLeft, originalFrustumRight, originalFrustumTop, originalFrustumBottom);

	Ogre::Real fullWidth = originalFrustumRight - originalFrustumLeft;
	Ogre::Real fullHeight = originalFrustumBottom - originalFrustumTop;


	Ogre::Real scaleX = (Ogre::Real)viewport->GetOgreViewport()->getActualWidth() / (Ogre::Real)viewport->GetVirtualWidth();
	scaleX /= viewport->GetScaleX();

	Ogre::Real scaleY = (Ogre::Real)viewport->GetOgreViewport()->getActualHeight() / (Ogre::Real)viewport->GetVirtualHeight();
	scaleY /= viewport->GetScaleY();


	Ogre::Real offsetX = (Ogre::Real)viewport->GetScrollOffsetX() / (Ogre::Real)viewport->GetVirtualWidth();
	Ogre::Real offsetY = (Ogre::Real)viewport->GetScrollOffsetY() / (Ogre::Real)viewport->GetVirtualHeight();


	frustumLeft = originalFrustumLeft + offsetX * fullWidth;
	frustumRight = frustumLeft + scaleX * fullWidth;
	frustumTop = originalFrustumTop + offsetY * fullHeight;
	frustumBottom = frustumTop + scaleY * fullHeight;

	camera->setFrustumExtents(frustumLeft, frustumRight, frustumTop, frustumBottom);
}


} // namespace Wme
