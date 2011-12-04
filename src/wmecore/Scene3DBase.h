// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScene3DBase_H__
#define __WmeScene3DBase_H__


#include "Scene.h"


namespace Wme
{
	class NavMesh;
	class MaterialInstance;

	class WmeDllExport Scene3DBase : public Scene
	{
	public:

		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport SceneGraphListener
		{
		public:
			SceneGraphListener() {}
			virtual ~SceneGraphListener() {}

			virtual void OnSceneNodeAdded(Ogre::SceneNode* node) {}
			virtual void OnSceneNodeRemoving(Ogre::SceneNode* node) {}
		};


		//////////////////////////////////////////////////////////////////////////

		Scene3DBase();
		virtual ~Scene3DBase();

		virtual void Create();
		virtual void Update();
		virtual void Render(Viewport* viewport, Camera* camera);

		virtual void Shutdown();

		void ImportDotSceneFile(const WideString& fileName);
		NavMesh* GetNavMesh() const { return m_NavMesh; }

		virtual void GetPath(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, NavPath* path);

		virtual void OnSceneNodeCreated(Ogre::SceneNode* node);
		virtual void OnSceneNodeDestroyed(Ogre::SceneNode* node);

		void AddSceneGraphListener(SceneGraphListener* listener);
		void RemoveSceneGraphListener(SceneGraphListener* listener);

		Ogre::ColourValue GetAmbientLightColor() const { return m_AmbientLightColor; }
		void SetAmbientLightColor(Ogre::ColourValue& color);

		enum FogType
		{
			FOG_NONE,
			FOG_LINEAR,
			FOG_EXP,
			FOG_EXP2
		};

		FogType GetFogType() const { return m_FogType; }
		void SetFogType(FogType type);

		Ogre::ColourValue GetFogColor() const { return m_FogColor; }
		void SetFogColor(const Ogre::ColourValue& color);

		Ogre::Real GetFogDensity() const { return m_FogDensity; }
		void SetFogDensity(Ogre::Real density);

		Ogre::Real GetFogStart() const { return m_FogStart; }
		void SetFogStart(Ogre::Real fogStart);

		Ogre::Real GetFogEnd() const { return m_FogEnd; }
		void SetFogEnd(Ogre::Real fogEnd);




		MaterialInstance* GetSkyboxMaterial() const { return m_SkyboxMaterial; }
		void SetSkyboxMaterial(const WideString& fileName);
		void ReApplySkyboxMaterial();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeScene3DBase"; }

		// script interface
		virtual void RegisterScriptInterface();

		bool ScTest(Script* script, const WideString& methodName, bool async);

	protected:
		void OnSceneNodeAdded(Ogre::SceneNode* node);
		void OnSceneNodeRemoving(Ogre::SceneNode* node);

		bool SaveNodeToXml(TiXmlElement* rootNode, Ogre::SceneNode* sceneNode);
		bool LoadNodeFromXml(TiXmlElement* rootNode, Ogre::SceneNode* parentSceneNode);
		

	private:
		typedef std::vector<SceneGraphListener*> SceneGraphListenerList;
		SceneGraphListenerList m_SceneGraphListeners;

		void DeleteNodes(Ogre::SceneNode* parentNode);
		void RebuildOgreScene();

		NavMesh* m_NavMesh;
		Ogre::ColourValue m_AmbientLightColor;

		FogType m_FogType;
		Ogre::ColourValue m_FogColor;
		Ogre::Real m_FogDensity;
		Ogre::Real m_FogStart;
		Ogre::Real m_FogEnd;

		MaterialInstance* m_SkyboxMaterial;
	};
}

#endif // __WmeScene3DBase_H__
