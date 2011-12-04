// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Renderer.h"
#include "OgreFileSystemBridge.h"
#include "ContentManager.h"
#include "MainWindow.h"
#include "GameStartupSettings.h"

using namespace Ogre;

namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
Renderer::Renderer()
{
	m_Root = NULL;
	m_WmeArchiveFactory = NULL;

	m_ClearSceneManagerPassMaps = false;
	m_MeshCleanup = false;
	m_SkeletonCleanup = false;
}

//////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
	ShutdownOgre();
}

//////////////////////////////////////////////////////////////////////////
bool Renderer::Initialize()
{
	// initialize Ogre
	if (!InitializeOgre()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Renderer::ShutdownOgre()
{	
	if (m_Root) m_Root->removeFrameListener(this);

	SAFE_DELETE(m_Root);
	SAFE_DELETE(m_WmeArchiveFactory);
}

//////////////////////////////////////////////////////////////////////////
bool Renderer::InitializeOgre()
{
	try
	{
		ShutdownOgre();
		m_Root = new Root("", "");

		// file system glue
		m_WmeArchiveFactory = new WmeArchiveFactory();
		Ogre::ArchiveManager::getSingleton().addArchiveFactory(m_WmeArchiveFactory);
		Ogre::ResourceGroupManager* rgm = Ogre::ResourceGroupManager::getSingletonPtr();
		rgm->addResourceLocation("WmeFileSystem", "Wme");

#ifdef _DEBUG
		//m_Root->loadPlugin("Plugin_CgProgramManager_d");
		m_Root->loadPlugin("Plugin_OctreeSceneManager_d");
		m_Root->loadPlugin("RenderSystem_GL_d");
#	ifdef WIN32
		m_Root->loadPlugin("RenderSystem_Direct3D9_d");
#	endif
#else
		//m_Root->loadPlugin("Plugin_CgProgramManager");
		m_Root->loadPlugin("Plugin_OctreeSceneManager");
		m_Root->loadPlugin("RenderSystem_GL");
#	ifdef WIN32
		m_Root->loadPlugin("RenderSystem_Direct3D9");
#	endif
#endif

		// TODO: let user set params
		// gotta reorganize things to allow the config dialog poll render system capabilities
		const RenderSystemList& renderSystems = m_Root->getAvailableRenderers();
		RenderSystemList::const_iterator it = renderSystems.begin();
		RenderSystem* renderSystem = NULL;

		while (it != renderSystems.end())
		{
			renderSystem = *(it++);
			if (renderSystem->getName().find("GL") != std::string::npos)
			//if (renderSystem->getName().find("3D9") != std::string::npos)
			{
				m_Root->setRenderSystem(renderSystem);
				break;
			}
		}
		if (!renderSystem) return false;

		m_Root->initialise(false);
		m_Root->addFrameListener(this);

		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.getFullDescription() << std::endl;
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
bool Renderer::ProcessFrame()
{
	if (m_MeshCleanup)
	{
		m_MeshCleanup = false;
		Ogre::MeshManager::getSingleton().removeUnreferencedResources();
	}

	if (m_SkeletonCleanup)
	{
		m_SkeletonCleanup = false;
		Ogre::SkeletonManager::getSingleton().removeUnreferencedResources();
	}

	ClearSceneManagerPassMaps();
	return m_Root->renderOneFrame();
}

//////////////////////////////////////////////////////////////////////////
bool Renderer::frameStarted(const Ogre::FrameEvent& evt)
{
	Game::GetInstance()->DisplayContent();
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Renderer::OnRenderQueueStart(Ogre::uint8 renderQueueId, Ogre::Viewport* viewport)
{
	Game::GetInstance()->GetContentMgr()->OnRenderQueueStart(renderQueueId, viewport);
}


//////////////////////////////////////////////////////////////////////////
void Renderer::ClearSceneManagerPassMaps()
{
	if (!m_ClearSceneManagerPassMaps) return;

	m_ClearSceneManagerPassMaps = false;
	if (m_Root)
	{
		Ogre::SceneManagerEnumerator::SceneManagerIterator scenesIter = m_Root->getSceneManagerIterator();
		while (scenesIter.hasMoreElements())
		{ 
			Ogre::SceneManager* pScene = scenesIter.getNext(); 
			if (pScene)
			{ 
				Ogre::RenderQueue* pQueue = pScene->getRenderQueue(); 
				if (pQueue)
				{
					pQueue->clear(true);
				}
			}
		}
		Ogre::Pass::processPendingPassUpdates();
	}    
}




} // namespace Wme
