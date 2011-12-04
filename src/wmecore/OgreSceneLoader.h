// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeOgreSceneLoader_H__
#define __WmeOgreSceneLoader_H__

#include "Object.h"
#include "OgreScriptCompiler.h"

namespace Wme
{
	class Ogre::Entity;
	class Ogre::Light;
	class Scene3DBase;
	class LightEntity;

	class WmeDllExport OgreSceneLoader :	public Object, public Ogre::ScriptCompilerListener
	{
	public:
		OgreSceneLoader();
		virtual ~OgreSceneLoader();

		bool LoadFile(const WideString& fileName, Scene3DBase* stage, Ogre::SceneNode* rootNode, const WideString& resourceGroup = L"General");

		// Ogre::ScriptCompilerListener
		virtual bool handleEvent(Ogre::ScriptCompiler* compiler, Ogre::ScriptCompilerEvent* evt, void* retval);

	private:
		class WmeDllExport TrackTarget
		{
		public:
			TrackTarget(Ogre::SceneNode* node)
			{
				SourceNode = node;
				Offset = LocalDirection = Ogre::Vector3::ZERO;
			}

			Ogre::SceneNode* SourceNode;
			Ogre::Vector3 Offset;
			Ogre::Vector3 LocalDirection;
			WideString NodeName;
		};

		class WmeDllExport LookTarget
		{
		public:
			LookTarget(Ogre::SceneNode* node)
			{
				SourceNode = node;
				LocalDirection = Position = Ogre::Vector3::ZERO;
				HasPosition = false;
				RelativeTo = Ogre::Node::TS_PARENT;
			}

			Ogre::SceneNode* SourceNode;
			Ogre::Vector3 LocalDirection;
			Ogre::Vector3 Position;
			bool HasPosition;
			WideString NodeName;
			Ogre::Node::TransformSpace RelativeTo;
		};

		bool LoadNodes(TiXmlElement* nodesElem, Ogre::SceneNode* parentNode);
		bool LoadNode(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode);
		bool LoadEnvironment(TiXmlElement* envElem);
		bool LoadEntity(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode);
		bool LoadSubEntities(TiXmlElement* nodeElem, Ogre::Entity* entity);
		bool LoadCamera(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode);
		bool LoadLight(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode);
		bool LoadLightRange(TiXmlElement* nodeElem, LightEntity* light);
		bool LoadLightAttenuation(TiXmlElement* nodeElem, LightEntity* light);
		bool LoadTrackTarget(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode);
		bool LoadLookTarget(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode);

		void ProcessLookTrackTargets();


		Ogre::Vector3 GetVector3(TiXmlElement* elem);
		Ogre::Quaternion GetRotation(TiXmlElement* elem);
		Ogre::ColourValue GetColor(TiXmlElement* elem);

		Scene3DBase* m_Stage;
		WideString m_FileName;
		WideString m_ResourceGroup;

		typedef std::list<TrackTarget> TrackTargetList;
		TrackTargetList m_TrackTargets;

		typedef std::list<LookTarget> LookTargetList;
		LookTargetList m_LookTargets;
	};
}


#endif // __WmeOgreSceneLoader_H__
