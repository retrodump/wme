// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeView_H__
#define __WmeView_H__


#include "Object.h"
#include "MouseEvent.h"

namespace Wme
{
	class Viewport;
	class Ogre::RenderTarget;
	class Ogre::Viewport;
	class ViewportLayout;

	/** Abstraction of render target */
	class WmeDllExport View : public Object, public Ogre::RenderTargetListener
	{
	public:
		View();
		virtual ~View();

		virtual void Initialize();

		void Update();

		void RemoveAllOgreViewports();
		void AddViewportToMap(Viewport* viewport, Ogre::Viewport* ogreViewport);

		virtual void GetMousePos(int& x, int& y) const = 0;
		virtual void SetMousePos(int x, int y) = 0;
		virtual Ogre::RenderTarget* GetRenderTarget() = 0;
		virtual Ogre::RenderWindow* GetOgreWindow() = 0;

		virtual bool IsShiftDown() const { return false; }
		virtual bool IsControlDown() const { return false; }
		virtual bool IsAltDown() const { return false; }		

		WideString GetName() const { return m_Name; }

		bool IsLevelWindow() const { return m_TopLevelWindow; }
		void SetTopLevelWindow(bool val) { m_TopLevelWindow = val; }


		ViewportLayout* GetViewportLayout() { return m_ViewportLayout; }

		void OnRenderQueueStart(Ogre::uint8 renderQueueId, Ogre::Viewport* viewport);

		Viewport* OgreToWmeViewport(Ogre::Viewport* ogreViewport);

		// Ogre::RenderTargetListener implementation
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		virtual void postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);


	protected:		
		WideString m_Name;
		bool m_TopLevelWindow;
		ViewportLayout* m_ViewportLayout;

	private:
		typedef std::map<Ogre::Viewport*, Viewport*> ViewportMap;
		ViewportMap m_ViewportMap;				
	};
}

#endif // __WmeView_H__
