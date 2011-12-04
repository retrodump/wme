// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeContentManager_H__
#define __WmeContentManager_H__


#include "Object.h"
#include "IMouseHandler.h"
#include "KeyboardEvent.h"
#include <boost/serialization/access.hpp>


namespace Wme
{
	class Stage;
	class View;
	class MainWindow;
	class EmbeddedWindow;
	class EmbeddedWindowBridge;
	class RttView;
	class GuiStage;
	class DebugHUD;
	class HybridScene;
	class FullScene;
	class ViewportLayout;
	class GameStartupSettings;
	class Camera;
	class HybridCamera;
	class InteractiveObject;
	class Ogre::Entity;
	class Ogre::Viewport;

	/** This class is responsible for holding the content - both stages and viewports. */
	class WmeDllExport ContentManager : public Object
	{
	public:
		ContentManager();
		~ContentManager();

		void Initialize(bool standalone);

		void UpdateContent();
		void DisplayContent();

		GuiStage* AddGuiStage();
		DebugHUD* AddDebugHUD();
		HybridScene* AddHybridScene();
		FullScene* AddFullScene();
		void RemoveStage(Stage* stage);

		MainWindow* AddMainWindow(const WideString& name, const GameStartupSettings& startupSettings);
		RttView* AddRttView(const WideString& name, size_t width, size_t height);
		EmbeddedWindow* AddEmbeddedWindow(const WideString& name, EmbeddedWindowBridge* brige);
		void RemoveView(View* view);

		MainWindow* GetMainWindow() { return m_MainWindow; }

		void OnCameraDestroyed(Camera* camera);

		void InvalidateViewports();
		void OnResizeViewports();

		View* GetViewByName(const WideString& name) const;

		InteractiveObject* GetActiveObject();
		InteractiveObject* GetActiveObject(View* view);

		// rendering
		bool ProcessFrame();
		void OnRenderQueueStart(Ogre::uint8 renderQueueId, Ogre::Viewport* viewport);

		// event related
		void InvokeMouseEvent(Ogre::RenderWindow* window, MouseEvent& event);
		void InvokeMouseEvent(View* view, MouseEvent& event);
		void CaptureMouse(IMouseHandler* captor, Viewport* viewport = NULL);
		void ReleaseMouse(IMouseHandler* captor);
		IMouseHandler* GetMouseCapture() const { return m_MouseCaptor; }
		void InvokeKeyboardEvent(View* view, KeyboardEvent& event);

		bool IsShiftDown();
		bool IsControlDown();
		bool IsAltDown();

		// temp
		void TestData(View* testView);
		FullScene* GetTestScene() { return m_TestScene; }

	private:
		friend class boost::serialization::access; 
		template <class Archive>void serialize(Archive& ar, const unsigned int version);

		typedef std::list<Stage*> StageList;
		StageList m_Stages;

		typedef std::list<View*> ViewList;
		ViewList m_Views;

		MainWindow* m_MainWindow;

		typedef std::map<Ogre::RenderWindow*, View*> ViewMap;
		ViewMap m_ViewMap;

		IMouseHandler* m_MouseCaptor;
		Viewport* m_MouseCaptorViewport;



		FullScene* m_TestScene;
	};
}


#endif // __WmeContentManager_H__