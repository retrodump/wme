// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "DebugHUD.h"
#include "DebugManager.h"

// temp
#include "Viewport.h"
#include "OgreViewport.h"
#include "ElementCollection.h"
#include "SpriteSubFrame.h"
#include "TextElement.h"
#include "LineElement.h"
#include "Renderer.h"
#include "StringUtil.h"
#include "Camera.h"
#include "ContentManager.h"
#include "ViewportLayout.h"
#include "Entity3D.h"
#include "Region.h"
#include "ActiveSpot.h"
#include "ResizableImageOld.h"
#include "UiWindowOld.h"
#include "UiButtonOld.h"
#include "Font.h"
#include "FontManager.h"
#include "View.h"
#include "EmbeddedWindow.h"
#include "SpriteFrame.h"
#include "SpriteSubFrame.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
DebugHUD::DebugHUD()
{
	m_ElementCol = NULL;
	m_QuickMsg = NULL;
	m_Arrow = NULL;
	m_Fps = NULL;
	m_Info = NULL;
	m_TestObj = NULL;
	m_TestSprite = NULL;
	m_Back = NULL;
	m_TestRegion = NULL;
	m_Window = NULL;
	m_Font = NULL;
}

//////////////////////////////////////////////////////////////////////////
DebugHUD::~DebugHUD()
{
	SAFE_DELETE(m_QuickMsg);
	SAFE_DELETE(m_Arrow);
	SAFE_DELETE(m_Fps);
	SAFE_DELETE(m_Info);
	SAFE_DELETE(m_Window);
	SAFE_DELETE(m_ElementCol);
	SAFE_DELETE(m_TestSprite);
	SAFE_DELETE(m_Back);
	SAFE_DELETE(m_TestObj);
	SAFE_DELETE(m_TestRegion);

	Game::GetInstance()->GetFontMgr()->ReleaseFont(m_Font);
}

//////////////////////////////////////////////////////////////////////////
void DebugHUD::Create()
{
	GuiStage::Create();

	m_TestObj = new Entity3D();
	m_TestObj->SetName(L"molly");

	m_TestSprite = new Sprite(m_TestObj);
	//bool b = m_TestSprite->LoadFromFile(L"actors/molly/walk/walk.sprite");
	bool b = m_TestSprite->LoadFromFile(L"actors/molly/walk/walk_frame.sprite");

	//m_TestSprite->SaveToFile(L"F:/projects/wme2/bin/data/actors/molly/walk/test.sprite");

	m_ElementCol = new ElementCollection();

	Ogre::SceneNode* spriteNode = GetRootNode()->createChildSceneNode();
	spriteNode->attachObject(m_ElementCol);


	m_Back = new Sprite(m_TestObj);
	m_Back->LoadFromFile(L"background.jpg");


	m_Font = Game::GetInstance()->GetFontMgr()->GetFont(L"arial_small.font", true);

	m_Fps = new TextElement();
	m_Fps->SetFont(m_Font);
	/*
	m_Fps->SetPosX(800);
	m_Fps->SetHorizontalAlign(TextElement::HA_RIGHT);
	*/

	m_QuickMsg = new TextElement();
	m_QuickMsg->SetFont(m_Font);


	m_Info = new TextElement();
	m_Info->SetPosY(20);
	m_Info->SetFont(m_Font);

/*
	m_Lines = new LineElement();
	m_Lines->SetColor(Ogre::ColourValue(1, 0, 1, 1));
*/
	m_TestRegion = new Region();
	m_TestRegion->AddPoint(222, 246);
	m_TestRegion->AddPoint(312, 241);
	m_TestRegion->AddPoint(313, 426);
	m_TestRegion->AddPoint(222, 417);
	//reg.AddPoint(300, 100);

	//m_Lines->AddRegion(*m_TestRegion);


	/*
	m_Lines->AddLine(Ogre::Vector2(100, 100), Ogre::Vector2(100, 300));
	m_Lines->AddLine(Ogre::Vector2(100, 300), Ogre::Vector2(300, 300));
	m_Lines->AddLine(Ogre::Vector2(300, 300), Ogre::Vector2(300, 100));
	m_Lines->AddLine(Ogre::Vector2(300, 100), Ogre::Vector2(100, 100));
	*/



	m_Arrow = new SpriteSubFrame(NULL);
	m_Arrow->SetTexture(L"cur_arrow.png");
	m_Arrow->SetBasePoint(Ogre::Vector2(0.0, 0.0));


	m_Window = new UiWindowOld(this);
	m_Window->Create();
	m_Window->SetName(L"test window");
	m_Window->LoadFromFile(L"test.window");

	/*
	m_Window->SetSize(200, 100);
	m_Window->SetPos(100, 100);
	//m_Window->SetClipContent(false);


	UiWindow* win = new UiWindow(this);
	win->Create();
	win->SetSize(61, 61);
	win->SetPos(150, 50);
	win->SetName(L"child window");
	win->SetPixelPerfect(true);


	m_Window->AddChild(win);


	UiButton* button = new UiButton(this);
	button->Create();
	button->SetSize(80, 50);
	button->SetPos(5, 30);
	button->SetName(L"button");
	button->SetText(L"Button here");
	m_Window->AddChild(button);
	//button->SetDisabled(true);
	m_Window->SaveToFile(L"c:/test.window");
	*/
}

//////////////////////////////////////////////////////////////////////////
void DebugHUD::Render(Viewport* viewport, Camera* camera)
{
	GuiStage::Render(viewport, camera);

	m_ElementCol->ClearQueue();

	SpriteDrawingParams params;
	params.AbsolutePos = false;
	params.Parent = m_TestObj;
	params.Viewport = viewport;
	params.Rotation = 0;
	params.Scale = 1;
	
	params.Parent = NULL;
	params.Color = Ogre::ColourValue(1, 1, 1, 0.6f);

	m_Back->Update();
	//m_Back->Display(m_ElementCol, 0, 0, params);

	params.Parent = m_TestObj;
	params.Color = Ogre::ColourValue::White;
	params.Rotation = 30;
	m_TestSprite->Update();
	//m_TestSprite->Display(m_ElementCol, 400, 550, params);


	int mouseX, mouseY;
	//Game::GetInstance()->GetContentMgr()->GetMainWindow()->GetMousePos(mouseX, mouseY);
	//viewport->ScreenToViewport(mouseX, mouseY);
	viewport->GetMousePos(mouseX, mouseY);


	params.Parent = NULL;
	params.Rotation = 0;
	params.Scale = 1;

	WideString info;

	Ogre::RenderWindow* win = viewport->GetParentLayout()->GetParentView()->GetOgreWindow();
	if (win)
	{
		const Ogre::RenderTarget::FrameStats& stats = win->getStatistics();
		info += L"FPS: " + StringUtil::ToString(stats.lastFPS) + L" Polys: " + StringUtil::ToString(stats.triangleCount) + L" Batches: " + StringUtil::ToString(stats.batchCount);
	}
	info += L"  [" + StringUtil::ToString(mouseX) + L", " + StringUtil::ToString(mouseY) + L"]";

	if (Game::GetInstance()->GetActiveObject()) info += L"  Obj: " + Game::GetInstance()->GetActiveObject()->GetName();


	m_Fps->SetText(info);
	m_Fps->UpdateGeometry(viewport, true);
	m_ElementCol->AddElement(m_Fps);

	// display quick messages
	DebugManager::QuickMessages quickMessages = Game::GetInstance()->GetDebugMgr()->GetMessages();
	if (!quickMessages.empty())
	{
		WideString msgText;
		foreach (WideString msg, quickMessages)
		{
			msgText += msg + L"\n";
		}
		m_QuickMsg->SetText(msgText);
		m_QuickMsg->SetPosX(0);
		m_QuickMsg->SetPosY(50);
		m_QuickMsg->SetMaxWidth(viewport->GetOgreViewport()->getActualWidth());

		SpriteDrawingParams params;
		params.Viewport = viewport;
		params.AbsolutePos = true;
		m_QuickMsg->Display(m_ElementCol, params);
	}
	

	m_Info->SetText(Game::GetInstance()->m_TestInfo);
	m_Info->UpdateGeometry(viewport, true);
	m_ElementCol->AddElement(m_Info);


	//m_Lines->UpdateGeometry(viewport, false);
	//m_ElementCol->AddElement(m_Lines);
	//viewport->AddActiveSpot(new ActiveSpotRegion(viewport, m_TestObj, m_TestRegion, false));


	m_Window->Display(m_ElementCol, params);


	params.Color = Ogre::ColourValue(1, 1, 1, 1);
	params.AbsolutePos = true;
	m_Arrow->Display(m_ElementCol, mouseX, mouseY, params);
}

//////////////////////////////////////////////////////////////////////////
void DebugHUD::Update()
{
	m_Window->Update();

	GuiStage::Update();
}



} // namespace Wme
