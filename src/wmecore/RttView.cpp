// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "RttView.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
RttView::RttView()
{
	m_InjectedMouseX = m_InjectedMouseY = 0;
	m_TopLevelWindow = false;
}

//////////////////////////////////////////////////////////////////////////
RttView::~RttView()
{
	GetRenderTarget()->removeListener(this);
}

//////////////////////////////////////////////////////////////////////////
void RttView::Create(const WideString& name, size_t width, size_t height)
{
	m_Texture = Ogre::TextureManager::getSingleton().createManual(
		StringUtil::WideToUtf8(name), 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, 
		width, height,
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET
		);

	GetRenderTarget()->addListener(this);
	m_Name = name;
}

//////////////////////////////////////////////////////////////////////////
Ogre::RenderTarget* RttView::GetRenderTarget()
{
	if (m_Texture.isNull()) return NULL;
	else return m_Texture->getBuffer()->getRenderTarget();
}

//////////////////////////////////////////////////////////////////////////
void RttView::GetMousePos(int& x, int& y) const
{
	x = m_InjectedMouseX;
	y = m_InjectedMouseY;
}

//////////////////////////////////////////////////////////////////////////
void RttView::SetMousePos(int x, int y)
{
	m_InjectedMouseX = x;
	m_InjectedMouseY = y;
}

//////////////////////////////////////////////////////////////////////////
void RttView::InjectMousePos(float relativeX, float relativeY)
{
	m_InjectedMouseX = (int)(m_Texture->getWidth() * relativeX);
	m_InjectedMouseY = (int)(m_Texture->getHeight() * relativeY);
}


} // namespace Wme
