// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Scene3DBase.h"
#include "Scene3DEditor.h"
#include "NavMesh.h"
#include "OgreSceneLoader.h"
#include "Camera.h"
#include "Viewport.h"
#include "NavPath.h"
#include "StringUtil.h"
#include "XmlUtil.h"
#include "EntityFactory.h"
#include "Entity3DBase.h"
#include "MaterialManager.h"
#include "MaterialInstance.h"
#include "Renderer.h"
#include "ActiveSpot.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Scene3DBase::Scene3DBase()
{
	m_NavMesh = NULL;
	m_RootNode = NULL;

	m_AmbientLightColor = Ogre::ColourValue::White;

	m_FogType = FOG_NONE;
	m_FogColor = Ogre::ColourValue::White;
	m_FogDensity = 0.001f;
	m_FogStart = 0.0f;
	m_FogEnd = 1000.0f;

	m_SkyboxMaterial = NULL;
}

//////////////////////////////////////////////////////////////////////////
Scene3DBase::~Scene3DBase()
{
	if (m_SkyboxMaterial)
	{
		Game::GetInstance()->GetMaterialMgr()->ReleaseMaterial(m_SkyboxMaterial);
		m_SkyboxMaterial = NULL;
	}

	SAFE_DELETE(m_NavMesh);
	if (m_RootNode)
	{
		GetSceneMgr()->destroySceneNode(m_RootNode);
		m_RootNode = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
void Scene3DBase::Create()
{
	Scene::Create();	
	m_NavMesh = new NavMesh;
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::Update()
{
	Scene::Update();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::Shutdown()
{
	Scene::Shutdown();
	DeleteNodes(GetRootNode());
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::DeleteNodes(Ogre::SceneNode* parentNode)
{
	for (word i = 0; i < parentNode->numChildren(); i++)
	{
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(parentNode->getChild(i));
		DeleteNodes(node);

		Entity3DBase* entity = Entity3DBase::NodeToEntity(node);
		if (entity) delete entity;
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::RegisterActiveSpots(Viewport* viewport, Camera* camera, Ogre::uint8 renderQueueId)
{
	ActiveSpotGeometry* spot = new ActiveSpotGeometry(viewport, this, camera, renderQueueId);
	viewport->AddActiveSpot(spot);
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::Render(Viewport* viewport, Camera* camera)
{
	Scene::Render(viewport, camera);
	camera->AdjustToViewport(viewport);
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::ImportDotSceneFile(const WideString& fileName)
{
	OgreSceneLoader* loader = new OgreSceneLoader();
	loader->LoadFile(fileName, this, GetRootNode());
	delete loader;
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::GetPath(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, NavPath* path)
{
	path->Reset();

	NavMesh::PathPointList meshPath;
	GetNavMesh()->FindPath(startPoint, endPoint, meshPath);

	path->Reserve(meshPath.size());
	foreach (NavMesh::PathPoint point, meshPath)
	{
		path->AddPoint(point.Point);
	}
	path->SetReady(true);
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::OnSceneNodeCreated(Ogre::SceneNode* node)
{
	if (m_Editor && m_Editor->Is3DEditor())
	{
		static_cast<Scene3DEditor*>(m_Editor)->OnSceneNodeCreated(node);
	}

	foreach (SceneGraphListener* listener, m_SceneGraphListeners)
	{
		listener->OnSceneNodeAdded(node);
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::OnSceneNodeDestroyed(Ogre::SceneNode* node)
{
	if (m_Editor && m_Editor->Is3DEditor())
	{
		static_cast<Scene3DEditor*>(m_Editor)->OnSceneNodeDestroyed(node);
	}
	foreach (SceneGraphListener* listener, m_SceneGraphListeners)
	{
		listener->OnSceneNodeRemoving(node);
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::OnSceneNodeChanged(Ogre::SceneNode* node)
{
	if (m_Editor && m_Editor->Is3DEditor())
	{
		static_cast<Scene3DEditor*>(m_Editor)->OnSceneNodeChanged(node);
	}
	foreach (SceneGraphListener* listener, m_SceneGraphListeners)
	{
		listener->OnSceneNodeChanged(node);
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::OnSceneGraphDirty()
{
	if (m_Editor && m_Editor->Is3DEditor())
	{
		static_cast<Scene3DEditor*>(m_Editor)->OnSceneGraphDirty();
	}
	foreach (SceneGraphListener* listener, m_SceneGraphListeners)
	{
		listener->OnSceneGraphDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::AddSceneGraphListener(SceneGraphListener* listener)
{
	if (std::find(m_SceneGraphListeners.begin(), m_SceneGraphListeners.end(), listener) == m_SceneGraphListeners.end())
		m_SceneGraphListeners.push_back(listener);
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::RemoveSceneGraphListener(SceneGraphListener* listener)
{
	SceneGraphListenerList::iterator it = std::find(m_SceneGraphListeners.begin(), m_SceneGraphListeners.end(), listener);
	if (it != m_SceneGraphListeners.end()) m_SceneGraphListeners.erase(it);

}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::RebuildOgreScene()
{
	GetSceneMgr()->setAmbientLight(m_AmbientLightColor);
	
	Ogre::FogMode mode;
	switch (m_FogType)
	{
	case FOG_LINEAR:
		mode = Ogre::FOG_LINEAR;
		break;
	case FOG_EXP:
		mode = Ogre::FOG_EXP;
		break;
	case FOG_EXP2:
		mode = Ogre::FOG_EXP2;
		break;
	default:
		mode = Ogre::FOG_NONE;
	}
	GetSceneMgr()->setFog(mode, m_FogColor, m_FogDensity, m_FogStart, m_FogEnd);

	// skybox
	if (m_SkyboxMaterial)
	{
		try
		{
			// see http://www.ogre3d.org/mantis/view.php?id=130
			Game::GetInstance()->GetRenderer()->ScheduleClearSceneManagerPassMaps();

			GetSceneMgr()->setSkyBox(true, m_SkyboxMaterial->GetOgreMaterial()->getName());
		}
		catch (Ogre::InvalidParametersException&)
		{
			GetSceneMgr()->setSkyBox(false, "");
		}
	}
	else
	{
		GetSceneMgr()->setSkyBox(false, "");
	}
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetAmbientLightColor(Ogre::ColourValue& color)
{
	m_AmbientLightColor = color;
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetFogType(FogType type)
{
	m_FogType = type;
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetFogColor(const Ogre::ColourValue& color)
{
	m_FogColor = color;
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetFogDensity(Ogre::Real density)
{
	m_FogDensity = density;
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetFogStart(Ogre::Real fogStart)
{
	m_FogStart = fogStart;
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetFogEnd(Ogre::Real fogEnd)
{
	m_FogEnd = fogEnd;
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::SetSkyboxMaterial(const WideString& fileName)
{
	Game::GetInstance()->GetMaterialMgr()->ReleaseMaterial(m_SkyboxMaterial);
	m_SkyboxMaterial = Game::GetInstance()->GetMaterialMgr()->GetMaterial(fileName);
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
void Scene3DBase::ReApplySkyboxMaterial()
{
	RebuildOgreScene();
}

//////////////////////////////////////////////////////////////////////////
bool Scene3DBase::LoadFromXml(TiXmlElement* rootNode)
{
	Scene::LoadFromXml(rootNode);

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Nodes")
		{
			for (TiXmlElement* nodeElem = elem->FirstChildElement(); nodeElem != NULL; nodeElem = nodeElem->NextSiblingElement())
			{
				LoadNodeFromXml(nodeElem, GetRootNode());
			}
		}
		else if (elem->ValueStr() == "Environment")
		{
			for (TiXmlElement* envElem = elem->FirstChildElement(); envElem != NULL; envElem = envElem->NextSiblingElement())
			{
				if (envElem->ValueStr() == "AmbientLightColor")
				{
					m_AmbientLightColor = XmlUtil::GetColor(envElem);
				}
				else if (envElem->ValueStr() == "FogType")
				{
					Utf8String strType = XmlUtil::TextToUtf8(envElem);
					if (strType == "Linear") m_FogType = FOG_LINEAR;
					else if (strType == "Exp") m_FogType = FOG_EXP;
					else if (strType == "Exp2") m_FogType = FOG_EXP2;
					else m_FogType = FOG_NONE;
				}
				else if (envElem->ValueStr() == "FogColor")
				{
					m_FogColor = XmlUtil::GetColor(envElem);
				}
				else if (envElem->ValueStr() == "FogDensity")
				{
					m_FogDensity = XmlUtil::TextToFloat(envElem);
				}
				else if (envElem->ValueStr() == "FogRange")
				{
					m_FogStart = XmlUtil::AttrToFloat(envElem, "Start");
					m_FogEnd = XmlUtil::AttrToFloat(envElem, "End");
				}
			}
		}
	}
	RebuildOgreScene();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Scene3DBase::LoadNodeFromXml(TiXmlElement* rootNode, Ogre::SceneNode* parentSceneNode)
{
	Entity* entity = EntityFactory::GetInstance()->CreateInstance(rootNode->ValueStr());
	if (!entity) return false;

	// load the entity itself
	Entity3DBase* entity3D = static_cast<Entity3DBase*>(entity);

	entity3D->LoadFromXml(rootNode);
	entity3D->Register();
	entity3D->SetOwnedByStage(true);

	// put to stage
	if (parentSceneNode == GetRootNode()) entity3D->PutToStage(this);
	else entity3D->PutToStage(this, Entity3DBase::NodeToEntity(parentSceneNode));

	// load child nodes
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Nodes")
		{
			for (TiXmlElement* nodeElem = elem->FirstChildElement(); nodeElem != NULL; nodeElem = nodeElem->NextSiblingElement())
			{
				LoadNodeFromXml(nodeElem, entity3D->GetSceneNode());
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Scene3DBase::SaveToXml(TiXmlElement* rootNode)
{
	Scene::SaveToXml(rootNode);

	TiXmlElement* elem;

	// nodes
	TiXmlElement* nodesElem = XmlUtil::AddElem("Nodes", rootNode);

	for (word i = 0; i < GetRootNode()->numChildren(); i++)
	{
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(GetRootNode()->getChild(i));
		SaveNodeToXml(nodesElem, node);
	}

	// environment
	TiXmlElement* envElem = XmlUtil::AddElem("Environment", rootNode);

	elem = XmlUtil::AddElem("AmbientLightColor", envElem);
	XmlUtil::SetColor(elem, m_AmbientLightColor);

	elem = XmlUtil::AddElem("FogType", envElem);
	switch (m_FogType)
	{
	case FOG_NONE:
		XmlUtil::SetText(elem, "None");
		break;
	case FOG_LINEAR:
		XmlUtil::SetText(elem, "Linear");
		break;
	case FOG_EXP:
		XmlUtil::SetText(elem, "Exp");
		break;
	case FOG_EXP2:
		XmlUtil::SetText(elem, "Exp2");
		break;
	}

	elem = XmlUtil::AddElem("FogColor", envElem);
	XmlUtil::SetColor(elem, m_FogColor);

	elem = XmlUtil::AddElem("FogDensity", envElem);
	XmlUtil::SetText(elem, m_FogDensity);

	elem = XmlUtil::AddElem("FogRange", envElem);
	XmlUtil::SetAttr(elem, "Start", m_FogStart);
	XmlUtil::SetAttr(elem, "End", m_FogEnd);


	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Scene3DBase::SaveNodeToXml(TiXmlElement* rootNode, Ogre::SceneNode* sceneNode)
{
	// must be an entity
	Entity3DBase* entity = Entity3DBase::NodeToEntity(sceneNode);
	if (!entity) return false;

	if (!entity->IsOwnedByStage()) return false;

	// save the entity itself
	TiXmlElement* nodeElem = XmlUtil::AddElem(entity->GetDocRootName(), rootNode);
	entity->SaveToXml(nodeElem);

	// save child nodes
	word numNodes = sceneNode->numChildren();
	if (numNodes > 0)
	{
		TiXmlElement* elem = XmlUtil::AddElem("Nodes", nodeElem);
		for (word i = 0; i < numNodes; i++)
		{
			SaveNodeToXml(elem, static_cast<Ogre::SceneNode*>(sceneNode->getChild(i)));
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void Scene3DBase::RegisterScriptInterface()
{
	Scene::RegisterScriptInterface();

	RegisterScriptMethod(L"Test",	(MethodHandler)&Scene3DBase::ScTest);
}

//////////////////////////////////////////////////////////////////////////
bool Scene3DBase::ScTest(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(0);

	Entity::EntityList list;
	GetEntitiesByName(L"Camera", L"Camera01", list);

	Shutdown();
	LoadFromFile(L"c:/test.scene");

	script->GetStack()->PushNull();
	return true;
}


} // namespace Wme
