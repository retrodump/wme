// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Scene2DBase.h"
#include "SceneNode2D.h"
#include "Viewport.h"
#include "Canvas2D.h"
#include "ActiveSpot.h"

// temp
#include "SpriteEntity.h"
#include "Sprite.h"
#include "SpriteFrame.h"
#include "SpriteSubFrame.h"
#include "TextureElement2D.h"
#include "UiButtonOld.h"
#include "TextElement2D.h"
#include "FontManager.h"
#include "RectElement2D.h"
#include "SpriteTexture.h"
#include "ResizableElement2D.h"
#include "Window.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Scene2DBase::Scene2DBase()
{
	m_Viewport = NULL;
	m_Canvas = NULL;

	// temp
	m_TestSprite = NULL;
	m_TestEntity = NULL;
	m_Tex = NULL;
	m_Font = NULL;
	m_Text = NULL;
	m_TestNode = NULL;
	m_Rect = NULL;
	m_RectNode = NULL;
	m_Resizable = NULL;
	m_ResizableImage = NULL;
	m_ResizableNode = NULL;
	m_Window1 = m_Window2 = NULL;
}

//////////////////////////////////////////////////////////////////////////
Scene2DBase::~Scene2DBase()
{
	SAFE_DELETE(m_Tex);
	SAFE_DELETE(m_TestEntity);
	SAFE_DELETE(m_TestSprite);
	SAFE_DELETE(m_Text);	
	SAFE_DELETE(m_TestNode);
	SAFE_DELETE(m_Rect);
	SAFE_DELETE(m_RectNode);
	SAFE_DELETE(m_Resizable);
	SAFE_DELETE(m_ResizableImage);
	SAFE_DELETE(m_ResizableNode);
	SAFE_DELETE(m_Window1);
	Game::GetInstance()->GetFontMgr()->ReleaseFont(m_Font);

	SAFE_DELETE(m_Canvas);
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Create()
{
	Scene::Create();

	m_Canvas = new Canvas2D(GetUniqueMovableName("Canvas2D", L"Canvas2D"), m_Viewport);
	this->GetRootNode()->attachObject(m_Canvas);

	// test data
	/*
	static UiButton dummy(NULL);
	dummy.SetName(L"molly");

	m_Tex = new TextureElement2D();
	m_Tex->SetSubFrame(m_TestSprite->GetFrames().at(0)->GetSubFrames().at(0));
	m_Tex->SetColor(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));
	m_Tex->SetOwner(&dummy);
	m_Canvas->GetRootNode()->AttachElement(m_Tex);
	*/
	//m_Canvas->GetRootNode()->SetClippingRect(Rect(-25, -50, 25, 50));

	m_TestEntity = new SpriteEntity();
	m_TestEntity->SetName(L"molly entity");
	m_TestEntity->PutToStage(this);

	m_TestSprite = new Sprite(m_TestEntity);
	m_TestSprite->LoadFromFile(L"actors/molly/walk/walk.sprite");

	m_TestEntity->SetCurrentSprite(m_TestSprite);
	m_TestEntity->SetPosition(400, 400);
	m_TestEntity->GetSceneNode()->SetClippingRect(Rect(0, -222, 50, 30));
	//m_TestEntity->SetRotation(270.0f);

	m_Font = Game::GetInstance()->GetFontMgr()->GetFont(L"arial.font", false);

	m_Text = new TextElement2D();
	m_Text->SetFont(m_Font);
	m_Text->SetText(L"CCCC\nTest test test.\nSecond line.");
	m_Text->SetWidth(500);
	m_Text->SetHeight(500);
	m_Text->SetLeadingSpace(20);
	m_Text->SetUnderline(true);
	m_Text->SetDecorationType(TextElement2D::DECORATION_OUTLINE);
	m_Text->SetDecorationColor(Ogre::ColourValue::Red);

	m_TestNode = m_Canvas->GetRootNode()->CreateChildNode();
	m_TestNode->AttachElement(m_Text);

	m_TestNode->SetPosition(400, 500);
	//m_TestNode->SetRotation(10);
	//m_TestNode->SetScale(2, 2);


	m_Rect = new RectElement2D();
	m_Rect->SetWidth(100);
	m_Rect->SetHeight(80);
	m_Rect->SetStrokeThickness(10);
	m_Rect->SetFillColor(Ogre::ColourValue(0, 1, 0, 0.5f));
	m_Rect->SetStrokeColor(Ogre::ColourValue::Red);
	m_RectNode = m_Canvas->GetRootNode()->CreateChildNode();
	m_RectNode->AttachElement(m_Rect);
	m_RectNode->SetPosition(300, 300);
	m_RectNode->SetRotation(45);
	//m_RectNode->SetScale(10, 10);


	m_ResizableImage = new ResizableImage();
	m_ResizableImage->SetTexture(L"win_grid.bmp");
	m_ResizableImage->SetFrameLeftWidth(30);
	m_ResizableImage->SetFrameRightWidth(30);
	m_ResizableImage->SetFrameTopHeight(30);
	m_ResizableImage->SetFrameBottomHeight(30);
	m_ResizableImage->SetPaintMiddlePart(true);
	m_ResizableImage->SetStretchHorizontal(true);
	m_ResizableImage->SetStretchVertical(true);

	m_Resizable = new ResizableElement2D();
	m_Resizable->SetWidth(200);
	m_Resizable->SetHeight(150);
	m_Resizable->SetImage(m_ResizableImage);

	m_ResizableNode = m_Canvas->GetRootNode()->CreateChildNode();
	m_ResizableNode->AttachElement(m_Resizable);
	m_ResizableNode->SetPosition(400, 30);

	m_Window1 = new Window(m_Canvas);
	m_Window1->SetClipChildren(true);
	m_Window1->SetPosX(60);
	m_Window1->SetPosY(440);
	m_Window1->SetWidth(200);
	m_Window1->SetHeight(200);

	m_Window2 = new Window(m_Canvas);
	m_Window2->SetPosX(150);
	m_Window2->SetPosY(150);
	m_Window2->SetWidth(100);
	m_Window2->SetHeight(100);

	m_Window1->AddChild(m_Window2);
/*
	m_Window2->SetAnchor(UiObjectBase::ANCHOR_LEFT, m_Window1, 10);
	m_Window2->SetAnchor(UiObjectBase::ANCHOR_RIGHT, m_Window1, 10);
	m_Window2->SetAnchor(UiObjectBase::ANCHOR_TOP, m_Window1, 10);
	m_Window2->SetAnchor(UiObjectBase::ANCHOR_BOTTOM, m_Window1, 10);
*/
	m_Window2->SetAnchor(UiObjectBase::ANCHOR_VERTICAL_CENTER, m_Window1);
	m_Window2->SetAnchor(UiObjectBase::ANCHOR_HORIZONTAL_CENTER, m_Window1);

	m_Window1->SetWidth(300);
	m_Window1->SetHeight(300);


	/*
	m_Canvas->GetRootNode()->SetRotation(45);
	m_Canvas->GetRootNode()->SetPosition(400, 400);
	m_Canvas->GetRootNode()->SetScale(2, 1);
	*/

	//m_ElementCol->getParentSceneNode()->attachObject(m_Canvas);

}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Update()
{
	Scene::Update();
	if (m_TestEntity) m_TestEntity->Update();
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
