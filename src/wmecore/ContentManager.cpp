// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ContentManager.h"
#include "Game.h"
#include "Renderer.h"
#include "Viewport.h"
#include "Stage.h"
#include "View.h"
#include "MainWindow.h"
#include "EmbeddedWindow.h"
#include "EmbeddedWindowBridge.h"
#include "RttView.h"
#include "GameStartupSettings.h"
#include "GuiStage.h"
#include "DebugHUD.h"
#include "HybridScene.h"
#include "FullScene.h"
#include "Scene2DBase.h"
#include "ViewportLayout.h"
#include "Camera.h"
#include "HybridCamera.h"
#include "FocusCameraController.h"
#include "InteractiveObject.h"
#include "serialize.h"
#include "CameraEntity.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ContentManager::ContentManager()
{
	m_MainWindow = NULL;
	m_MouseCaptor = NULL;
	m_MouseCaptorViewport = NULL;


	m_TestScene = NULL;
}

//////////////////////////////////////////////////////////////////////////
ContentManager::~ContentManager()
{
	m_MainWindow = NULL;
	foreach (View* view, m_Views)
	{
		SAFE_DELETE(view);
	}
	m_Views.clear();

	foreach (Stage* stage, m_Stages)
	{
		stage->Shutdown();
		SAFE_DELETE(stage);
	}
	m_Stages.clear();
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::Initialize(bool standalone)
{
	// temporary
	if (standalone)
	{
		GameStartupSettings startupSettings;
		AddMainWindow(L"MainWindow", startupSettings);
	}
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::TestData(View* testView)
{
	ViewportLayout* layout;

	bool autoScale = true;

	Viewport* vp;
/*
	HybridScene* scene = AddHybridScene();	
	vp = layout->AddViewport(L"", scene->GetDefaultCamera(), 0, 0, 1, 0.5);
	vp->SetDimensions(800, 600);


	vp = layout->AddViewport(L"", scene->GetDefaultCamera(), 0, 0.5, 1, 0.5);
	vp->SetDimensions(800, 600);
	//vp->SetScrollOffsetY(150);
*/
	DebugHUD* hud;

	// RTT
	RttView* rttView = AddRttView(L"RttTest", 512, 512);
	layout = rttView->GetViewportLayout();

	hud = AddDebugHUD();
	vp = layout->AddViewport(L"RttGUI", hud->GetDefaultCamera(), 0, 0, 1, 1);
	vp->SetDimensions(800, 600);
	vp->SetAutoScale(false);	
	layout->RebuildViewportStack();


	// 3d scene
	layout = testView->GetViewportLayout();

	m_TestScene = AddFullScene();
	CameraEntity* cam = static_cast<CameraEntity*>(m_TestScene->GetEntityByName(L"Camera", L"Camera01"));
/*
	vp = layout->AddViewport(L"Geometry", cam->GetCamera(), 0, 0, 1, 1);
	vp->SetDimensions(800, 600, 800, 600);	
	vp->SetAutoScale(autoScale);

	FocusCameraController* controller = new FocusCameraController();
	controller->Create(vp);

	vp->SetCameraController(controller);
*/



	// 2d scene
	Scene2DBase* scene2d = Add2DScene();
	vp = layout->AddViewport(L"2D", scene2d->GetDefaultCamera(), 0, 0, 1, 1);
	vp->SetDimensions(800, 600);
	vp->SetAutoScale(false);	
	layout->RebuildViewportStack();
	scene2d->SetViewport(vp);


/*
	Camera* cam2 = m_TestScene->AddHybridCamera(L"Camera02");
	vp = layout->AddViewport(L"Geometry2", cam2, 0.5, 0, 0.5, 1);
	vp->SetDimensions(800, 600, 800, 600);	
	vp->SetAutoScale(autoScale);

	controller = new FocusCameraController(vp);
	vp->SetCameraController(controller);
*/
/*
	Camera* cam2 = m_TestScene->AddHybridCamera(L"Camera02");
	vp = layout->AddViewport(L"Geometry2", cam2, 0.7, 0.7, 0.29f, 0.29f);
	vp->SetDimensions(800, 600, 800, 600);	
	vp->SetAutoScale(autoScale);

	controller = new FocusCameraController(vp);
	vp->SetCameraController(controller);
*/

	/*
	vp = layout->AddViewport(L"", m_TestCamera, 0, 0, 0.5, 1);
	vp = layout->AddViewport(L"", m_TestCamera, 0.5, 0, 0.5, 1);
	*/


	GuiStage* gui = AddGuiStage();
	vp = layout->AddViewport(L"GUI", gui->GetDefaultCamera(), 0, 0, 1, 1);
	vp->SetDimensions(800, 600, 800, 600);
	vp->SetAutoScale(autoScale);
	vp->SetClearBeforePaint(false);
	//vp->SetScrollOffsetX(0);
	//vp->SetScrollOffsetY(0);


	hud = AddDebugHUD();
	vp = layout->AddViewport(L"HUD", hud->GetDefaultCamera(), 0, 0, 1, 1);
	vp->SetDimensions(800, 600);
	vp->SetScaleFactor(1.0);
	vp->SetAutoScale(false);
	vp->SetClearBeforePaint(false);
	//vp->SetScrollOffsetX(0);
	//vp->SetScrollOffsetY(0);


	//m_TestCamera->AdjustToViewport(vp);

	layout->RebuildViewportStack();	
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::OnCameraDestroyed(Camera* camera)
{
	foreach (View* view, m_Views)
	{
		ViewportLayout* layout = view->GetViewportLayout();
		if (layout) layout->OnCameraDestroyed(camera);
	}
}


//////////////////////////////////////////////////////////////////////////
void ContentManager::UpdateContent()
{
	foreach (Stage* stage, m_Stages)
	{
		stage->Update();
	}

	foreach (View* view, m_Views)
	{
		view->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::DisplayContent()
{
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::InvalidateViewports()
{
	foreach (View* view, m_Views)
	{
		ViewportLayout* layout = view->GetViewportLayout();
		if (layout) layout->InvalidateViewports();
	}
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::OnResizeViewports()
{
	foreach (View* view, m_Views)
	{
		ViewportLayout* layout = view->GetViewportLayout();
		if (layout) layout->OnResizeViewports();
	}
}

//////////////////////////////////////////////////////////////////////////
Scene2DBase* ContentManager::Add2DScene()
{
	Scene2DBase* scene = new Scene2DBase();
	scene->Register();
	scene->Create();

	m_Stages.push_back(scene);

	return scene;
}

//////////////////////////////////////////////////////////////////////////
GuiStage* ContentManager::AddGuiStage()
{
	GuiStage* stage = new GuiStage();
	stage->Register();
	stage->Create();

	m_Stages.push_back(stage);

	return stage;
}

//////////////////////////////////////////////////////////////////////////
DebugHUD* ContentManager::AddDebugHUD()
{
	DebugHUD* hud = new DebugHUD();
	hud->Register();
	hud->Create();

	m_Stages.push_back(hud);

	return hud;
}

//////////////////////////////////////////////////////////////////////////
HybridScene* ContentManager::AddHybridScene()
{
	HybridScene* scene = new HybridScene();
	scene->Register();
	scene->Create();

	m_Stages.push_back(scene);

	return scene;
}

//////////////////////////////////////////////////////////////////////////
FullScene* ContentManager::AddFullScene()
{
	FullScene* scene = new FullScene();
	scene->Register();
	scene->Create();

	m_Stages.push_back(scene);

	return scene;
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::RemoveStage(Stage* stage)
{
	if (stage == NULL) return;

	StageList::iterator it;
	for (it = m_Stages.begin(); it != m_Stages.end(); it++)
	{
		if ((*it) == stage)
		{
			m_Stages.erase(it);
			stage->Shutdown();
			delete stage;
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
MainWindow* ContentManager::AddMainWindow(const WideString& name, const GameStartupSettings& startupSettings)
{
	RemoveView(m_MainWindow);

	m_MainWindow = new MainWindow();
	m_MainWindow->Create(name, startupSettings);
	m_MainWindow->Initialize();

	m_Views.push_back(m_MainWindow);
	m_ViewMap[m_MainWindow->GetOgreWindow()] = m_MainWindow;

	return m_MainWindow;
}

//////////////////////////////////////////////////////////////////////////
RttView* ContentManager::AddRttView(const WideString& name, size_t width, size_t height)
{
	RttView* view = new RttView();
	view->Create(name, width, height);
	view->Initialize();

	m_Views.push_back(view);

	return view;
}

//////////////////////////////////////////////////////////////////////////
EmbeddedWindow* ContentManager::AddEmbeddedWindow(const WideString& name, EmbeddedWindowBridge* brige)
{
	EmbeddedWindow* window = new EmbeddedWindow(brige);
	window->Create(name);
	window->Initialize();

	m_Views.push_back(window);
	m_ViewMap[window->GetOgreWindow()] = window;

	if (name != L"primaryEmbeddedWindow") TestData(window);

	return window;
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::RemoveView(View* view)
{
	if (view == NULL) return;
	if (view == m_MainWindow) m_MainWindow = NULL;


	ViewMap::iterator mapIt = m_ViewMap.find(view->GetOgreWindow());
	if (mapIt != m_ViewMap.end()) m_ViewMap.erase(mapIt);


	ViewList::iterator it;
	for (it = m_Views.begin(); it != m_Views.end(); it++)
	{
		if ((*it) == view)
		{
			m_Views.erase(it);
			delete view;
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
View* ContentManager::GetViewByName(const WideString& name) const
{
	foreach (View* view, m_Views)
	{
		if (view->GetName() == name) return view;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool ContentManager::ProcessFrame()
{
	if (m_MainWindow) m_MainWindow->ProcessFrame();

	UpdateContent();

	bool shutdown = false;
	bool render = false;

	// render stuff
	if (m_MainWindow)
	{
		Ogre::RenderWindow* win = m_MainWindow->GetOgreWindow();
		if (win)
		{
			if (win->isClosed()) shutdown = true;
			if (win->isActive()) render = true;
		}
	}
	else
	{
		if (m_Views.empty()) shutdown = true;
		else render = true;
	}
	
	if (render) Game::GetInstance()->GetRenderer()->ProcessFrame();

	return !shutdown;
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::OnRenderQueueStart(Ogre::uint8 renderQueueId, Ogre::Viewport* viewport)
{
	foreach (View* view, m_Views)
	{
		view->OnRenderQueueStart(renderQueueId, viewport);
	}
}

//////////////////////////////////////////////////////////////////////////
InteractiveObject* ContentManager::GetActiveObject()
{
	if (m_MainWindow) return GetActiveObject(m_MainWindow);
	else
	{
		foreach (View* view, m_Views)
		{
			if (!view->IsTopLevelWindow()) continue;

			InteractiveObject* actObj = GetActiveObject(view);
			if (actObj) return actObj;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
InteractiveObject* ContentManager::GetActiveObject(View* view)
{
	int mouseX, mouseY;
	view->GetMousePos(mouseX, mouseY);

	MousePickResult pickResult;
	ViewportLayout* layout = view->GetViewportLayout();

	if (layout && layout->GetObjectAt(mouseX, mouseY, pickResult))
	{
		return pickResult.Object;
	}
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::InvokeMouseEvent(Ogre::RenderWindow* window, MouseEvent& event)
{
	if (m_MouseCaptor) m_MouseCaptor->HandleMouseEvent(m_MouseCaptorViewport, event);
	else
	{
		ViewMap::iterator it = m_ViewMap.find(window);
		if (it != m_ViewMap.end())
		{
			it->second->GetViewportLayout()->HandleMouseEvent(event);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::InvokeMouseEvent(View* view, MouseEvent& event)
{
	view->GetViewportLayout()->HandleMouseEvent(event);
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::InvokeKeyboardEvent(View* view, KeyboardEvent& event)
{
	view->GetViewportLayout()->HandleKeyboardEvent(event);
}

//////////////////////////////////////////////////////////////////////////
bool ContentManager::IsShiftDown()
{
	foreach (View* view, m_Views)
	{
		if (view->IsTopLevelWindow()) return view->IsShiftDown();
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool ContentManager::IsControlDown()
{
	foreach (View* view, m_Views)
	{
		if (view->IsTopLevelWindow()) return view->IsControlDown();
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool ContentManager::IsAltDown()
{
	foreach (View* view, m_Views)
	{
		if (view->IsTopLevelWindow()) return view->IsAltDown();
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::CaptureMouse(IMouseHandler* captor, Viewport* viewport)
{
	m_MouseCaptor = captor;
	m_MouseCaptorViewport = viewport;
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::ReleaseMouse(IMouseHandler* captor)
{
	if (captor == m_MouseCaptor)
	{
		m_MouseCaptor = NULL;
		m_MouseCaptorViewport = NULL;
	}
}

SERIALIZE(ContentManager);
//////////////////////////////////////////////////////////////////////////
template <class Archive>void ContentManager::serialize(Archive& ar, const unsigned int version)
{
	WideString test = L"meh";
	ar & test;
}


} // namespace Wme
