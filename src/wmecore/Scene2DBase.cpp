// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Scene2DBase.h"
#include "SceneNode2D.h"
#include "Viewport.h"
#include "Canvas2D.h"
#include "ActiveSpot.h"

// temp
#include "Sprite.h"
#include "SpriteFrame.h"
#include "SpriteSubFrame.h"
#include "TextureElement2D.h"
#include "UiButton.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Scene2DBase::Scene2DBase()
{
	m_Viewport = NULL;
	m_Canvas = NULL;

	// temp
	m_TestSprite = NULL;
	m_Tex = NULL;
}

//////////////////////////////////////////////////////////////////////////
Scene2DBase::~Scene2DBase()
{
	SAFE_DELETE(m_Tex);
	SAFE_DELETE(m_TestSprite);

	SAFE_DELETE(m_Canvas);
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Create()
{
	Scene::Create();

	m_Canvas = new Canvas2D(GetUniqueMovableName("Canvas2D", L"Canvas2D"), m_Viewport);
	this->GetRootNode()->attachObject(m_Canvas);

	// test data
	static UiButton dummy(NULL);
	dummy.SetName(L"molly");

	m_TestSprite = new Sprite(NULL);
	m_TestSprite->LoadFromFile(L"actors/molly/walk/walk_frame.sprite");
		
	m_Tex = new TextureElement2D();
	m_Tex->SetSubFrame(m_TestSprite->GetFrames().at(0)->GetSubFrames().at(0));
	m_Tex->SetOwner(&dummy);
	m_Canvas->GetRootNode()->AttachElement(m_Tex);

	m_Canvas->GetRootNode()->SetRotation(45);
	m_Canvas->GetRootNode()->SetPosition(400, 400);
	m_Canvas->GetRootNode()->SetScale(2, 1);

	//m_Canvas->GetRootNode()->SetClippingRect(Rect(-25, -50, 25, 50));


	//m_ElementCol->getParentSceneNode()->attachObject(m_Canvas);

}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Update()
{
	Scene::Update();
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Shutdown()
{
	Scene::Shutdown();
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::RegisterActiveSpots(Viewport* viewport, Camera* camera, Ogre::uint8 renderQueueId)
{
	if (m_Canvas && viewport == m_Viewport && renderQueueId == m_Canvas->getRenderQueueGroup())
	{
		ActiveSpotCanvas* spot = new ActiveSpotCanvas(viewport, m_Canvas);
		viewport->AddActiveSpot(spot);
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::SetViewport(Viewport* viewport)
{
	m_Viewport = viewport;
	if (m_Canvas) m_Canvas->SetViewport(viewport);
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::OnSceneNodeCreated(SceneNode2D* node)
{

}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::OnSceneNodeDestroyed(SceneNode2D* node)
{

}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::OnSceneNodeChanged(SceneNode2D* node)
{

}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::OnSceneGraphDirty()
{

}


} // namespace Wme
