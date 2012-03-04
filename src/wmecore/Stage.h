// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeStage_h__
#define __WmeStage_h__


#include "InteractiveObject.h"
#include "MousePickParams.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "Entity.h"


namespace Wme
{
	class Viewport;
	class Camera;
	class HybridCamera;
	class View;
	class StageEditor;
	class NavPath;

	/** Abstraction of scene graph */
	class WmeDllExport Stage : public InteractiveObject, public Ogre::RenderQueueListener, public Ogre::SceneManager::Listener
	{
	public:

		typedef std::set<Entity*> EntitySet;

		Stage();
		virtual ~Stage();

		Ogre::SceneManager* GetSceneMgr() { return m_SceneMgr; }
		Ogre::SceneNode* GetRootNode() const;
		Ogre::SceneNode* GetSystemRootNode() const;

		virtual void Create();
		virtual void Destroy();

		virtual void Update();

		virtual void Shutdown();

		virtual void Render(Viewport* viewport, Camera* camera);

		virtual void RegisterActiveSpots(Viewport* viewport, Camera* camera, Ogre::uint8 renderQueueId) {}

		Camera* GetDefaultCamera() { return m_Default2DCamera; }
		
		Camera* AddCamera(const WideString& name);
		HybridCamera* AddHybridCamera(const WideString& name);
		
		void RemoveCamera(Camera* camera);
		void _DestroyOgreCamera(Camera* camera);

		virtual void ReleaseOgreViewports(Viewport* viewport) {};
		virtual void RebuildOgreViewports(int& zOrder, Viewport* viewport) {};

		virtual void OnViewportCreated(Viewport* viewport) {};

		bool RaycastFromPoint(MousePickParams& pickParams);

		void AddPreFindVisibleNotifyRequest(Entity* ent);
		void AddPostFindVisibleNotifyRequest(Entity* ent);

		virtual void OnAddEntity(Entity* entity);
		virtual void OnRemoveEntity(Entity* entity);
		virtual void OnRenameEntity(Entity* entity, const WideString& oldName, const WideString& newName);

		void GetEntitiesByName(const WideString& subType, const WideString& entityName, Entity::EntityList& entityList);
		Entity* GetEntityByName(const WideString& subType, const WideString& entityName);

		StageEditor* GetEditor() const { return m_Editor; }
		void SetEditor(StageEditor* editor);

		const EntitySet& GetContainedEntities() { return m_ContainedEntities; }

		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		virtual bool HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event) { return false; }

		virtual void GetPath(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, NavPath* path);

		WideString GetUniqueNodeName(const WideString& origName);
		WideString GetUniqueMovableName(const Utf8String& movableType, const WideString& origName);

        // ScriptableObject
		RTTI(Stage);
		virtual WideString ConvertToString() { return L"Stage object"; }
		virtual WideString GetTypeName() const { return L"Stage"; }

		// Ogre::RenderQueueListener implementation
		virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
		virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);

		// Ogre::SceneManager::Listener implementation
		virtual void preFindVisibleObjects(Ogre::SceneManager* source, Ogre::SceneManager::IlluminationRenderStage irs, Ogre::Viewport* v);
		virtual void postFindVisibleObjects(Ogre::SceneManager* source, Ogre::SceneManager::IlluminationRenderStage irs, Ogre::Viewport* v);

	protected:		
		Ogre::SceneManager* m_SceneMgr;
		Ogre::SceneNode* m_RootNode;
		Ogre::SceneNode* m_SystemRootNode;
		Camera* m_Default2DCamera;

		Ogre::RaySceneQuery* m_RaySceneQuery;

		typedef std::list<Camera*> CameraList;
		CameraList m_Cameras;

		typedef std::multimap<WideString, Entity*> EntityNameMap;
		typedef std::map<WideString, EntityNameMap*> EntityTypeMap;

		//typedef std::set<Entity*> EntitySet;
		EntitySet m_PreFindVisibleNotifyRequests;
		EntitySet m_PostFindVisibleNotifyRequests;

		EntityTypeMap m_NameTypeMap;
		
		EntitySet m_ContainedEntities;

		StageEditor* m_Editor;
	};
}


#endif // __WmeStage_h__
