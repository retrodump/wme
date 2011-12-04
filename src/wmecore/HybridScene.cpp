// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "HybridScene.h"
#include "ElementCollection.h"
#include "SpriteSubFrame.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
HybridScene::HybridScene()
{
	m_Elements = NULL;
	m_Background = NULL;
}

//////////////////////////////////////////////////////////////////////////
HybridScene::~HybridScene()
{
	SAFE_DELETE(m_Background);
	SAFE_DELETE(m_Elements);
}

//////////////////////////////////////////////////////////////////////////
void HybridScene::Create()
{
	Scene3DBase::Create();

	m_Elements = new ElementCollection;
	m_Elements->setRenderQueueGroup(10);

	Ogre::SceneNode* spriteNode = GetRootNode()->createChildSceneNode();
	spriteNode->attachObject(m_Elements);


	m_Background = new SpriteSubFrame();
	m_Background->SetTexture(L"background.jpg");

}


//////////////////////////////////////////////////////////////////////////
void HybridScene::Render(Viewport* viewport, Camera* camera)
{
	Scene3DBase::Render(viewport, camera);


	if (!m_Elements || ! m_Background) return;

	m_Elements->ClearQueue();

	SpriteDrawingParams params;
	params.Viewport = viewport;
	params.AbsolutePos = false;
	m_Background->Display(m_Elements, 0, 0, params);
}


} // namespace Wme
