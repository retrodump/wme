// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRenderer_H__
#define __WmeRenderer_H__

#include "Object.h"
#include "Game.h"

namespace Wme
{
	class Ogre::Root;
	class Ogre::Viewport;
	class WmeArchiveFactory;


	class WmeDllExport Renderer : public Object, public Ogre::FrameListener
	{
	public:
		Renderer();
		virtual ~Renderer();
		bool Initialize();
		bool ProcessFrame();

		void OnRenderQueueStart(Ogre::uint8 renderQueueId, Ogre::Viewport* viewport);

		void ScheduleClearSceneManagerPassMaps() { m_ClearSceneManagerPassMaps = true; }
		void ScheduleMeshCleanup() { m_MeshCleanup = true; }
		void ScheduleSkeletonCleanup() { m_SkeletonCleanup = true; }


		// Ogre::FrameListener implementation
		virtual bool frameStarted(const Ogre::FrameEvent& evt);

	private:
		void ClearSceneManagerPassMaps();
		void ShutdownOgre();
		bool InitializeOgre();
		Ogre::Root* m_Root;
		bool m_ClearSceneManagerPassMaps;
		bool m_MeshCleanup;
		bool m_SkeletonCleanup;

		Ogre::MeshPtr m_PrefabCube;

		WmeArchiveFactory* m_WmeArchiveFactory;
	};
}

#endif // __WmeRenderer_H__
