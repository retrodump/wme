// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "OgreSceneLoader.h"
#include "Game.h"
#include "Scene3DBase.h"
#include "CameraEntity.h"
#include "LightEntity.h"
#include "XmlUtil.h"
#include "StringUtil.h"
#include "FileManager.h"
#include "File.h"
#include "PathUtil.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
OgreSceneLoader::OgreSceneLoader()
{
	m_Stage = NULL;
}

//////////////////////////////////////////////////////////////////////////
OgreSceneLoader::~OgreSceneLoader()
{
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadFile(const WideString& fileName, Scene3DBase* stage, Ogre::SceneNode* rootNode, const WideString& resourceGroup)
{
	TiXmlDocument* doc = XmlUtil::OpenXmlFile(fileName, "scene");
	if (!doc) return false;

	m_Stage = stage;
	m_FileName = fileName;
	m_ResourceGroup = resourceGroup;


	WideString materialFile = PathUtil::GetFileNameWithoutExtension(m_FileName) + L".material";
	materialFile = PathUtil::Combine(PathUtil::GetDirectoryName(m_FileName), materialFile);
	if (Game::GetInstance()->GetFileMgr()->FileExists(materialFile))
	{
		Ogre::ScriptCompilerManager::getSingleton().setListener(this);

		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(StringUtil::WideToUtf8(materialFile));
		Ogre::MaterialManager::getSingleton().parseScript(stream, StringUtil::WideToUtf8(m_ResourceGroup));

		Ogre::ScriptCompilerManager::getSingleton().setListener(NULL);
	}
	

	for (TiXmlElement* elem = doc->RootElement()->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "environment")
		{
			LoadEnvironment(elem);
		}
		else if (elem->ValueStr() == "nodes")
		{
			LoadNodes(elem, rootNode);
		}
	}

	//ProcessLookTrackTargets();

	m_LookTargets.clear();
	m_TrackTargets.clear();
	delete doc;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadNodes(TiXmlElement* nodesElem, Ogre::SceneNode* parentNode)
{
	for (TiXmlElement* elem = nodesElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "node") LoadNode(elem, parentNode);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadNode(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode)
{
	WideString nodeName = XmlUtil::AttrToString(nodeElem, "name");

	Ogre::SceneNode* sceneNode;
	if (nodeName.empty()) sceneNode = parentNode->createChildSceneNode();
	else sceneNode = parentNode->createChildSceneNode(StringUtil::WideToUtf8(nodeName));

	sceneNode->setInheritScale(false);

	for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "node")
		{
			LoadNode(elem, sceneNode);
		}
		else if (elem->ValueStr() == "position")
		{
			sceneNode->setPosition(GetVector3(elem));
		}
		else if (elem->ValueStr() == "rotation" || elem->ValueStr() == "quaternion") // blender exporter exports "quaternion"
		{
			sceneNode->setOrientation(GetRotation(elem));
		}
		else if (elem->ValueStr() == "scale")
		{
			sceneNode->setScale(GetVector3(elem));
		}
		else if (elem->ValueStr() == "entity")
		{
			LoadEntity(elem, sceneNode);
		}
		else if (elem->ValueStr() == "camera")
		{
			LoadCamera(elem, sceneNode);
		}
		else if (elem->ValueStr() == "light")
		{
			LoadLight(elem, sceneNode);
		}
		else if (elem->ValueStr() == "trackTarget")
		{
			LoadTrackTarget(elem, sceneNode);
		}
		else if (elem->ValueStr() == "lookTarget")
		{
			LoadLookTarget(elem, sceneNode);
		}
	}


	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadEntity(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode)
{
	WideString name = XmlUtil::AttrToString(nodeElem, "name");
	WideString id = XmlUtil::AttrToString(nodeElem, "id");
	WideString meshFile = XmlUtil::AttrToString(nodeElem, "meshFile");
	WideString materialFile = XmlUtil::AttrToString(nodeElem, "materialFile");
	bool isStatic = XmlUtil::AttrToBool(nodeElem, "static", false);
	bool castShadows = XmlUtil::AttrToBool(nodeElem, "castShadows", true);


	Ogre::Entity *ent = NULL;
	try
	{
		if (!Game::GetInstance()->GetFileMgr()->FileExists(meshFile))
		{
			WideString basePath = PathUtil::GetDirectoryName(m_FileName);
			meshFile = PathUtil::Combine(basePath, PathUtil::GetFileName(meshFile));
		}


		Utf8String meshFileUtf8 = StringUtil::WideToUtf8(meshFile);

		Ogre::MeshManager::getSingleton().load(meshFileUtf8, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		ent = m_Stage->GetSceneMgr()->createEntity(StringUtil::WideToUtf8(name), meshFileUtf8);
		ent->setCastShadows(castShadows);
		parentNode->attachObject(ent);

		if(!materialFile.empty())
			ent->setMaterialName(StringUtil::WideToUtf8(materialFile));

		// subentities
		for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
		{
			if (elem->ValueStr() == "subentities") LoadSubEntities(elem, ent);
		}

	}
	catch(Ogre::Exception& e)
	{
		Game::GetInstance()->Log(L"Error loading entity", m_FileName);
		Game::GetInstance()->Log(e.getDescription());
		return false;
	}



	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadSubEntities(TiXmlElement* nodeElem, Ogre::Entity* entity)
{
	for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "subentity")
		{
			int index = XmlUtil::AttrToInt(elem, "index", -1);
			WideString materialName = XmlUtil::AttrToString(elem, "materialName");

			try
			{
				Ogre::SubEntity* subEnt = entity->getSubEntity(index);
				subEnt->setMaterialName(StringUtil::WideToUtf8(materialName));
			}
			catch (Ogre::Exception& e)
			{
				Game::GetInstance()->Log(L"Error setting material for subentity", m_FileName);
				Game::GetInstance()->Log(e.getDescription());
			}			
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadCamera(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode)
{
	WideString name = XmlUtil::AttrToString(nodeElem, "name");
	WideString id = XmlUtil::AttrToString(nodeElem, "id");
	float fov = XmlUtil::AttrToFloat(nodeElem, "fov", Ogre::Math::PI / 2);
	float aspectRatio = XmlUtil::AttrToFloat(nodeElem, "aspectRatio", 0);
	
	WideString projectionType;	
	if (nodeElem->Attribute("projectionType") != NULL)
		projectionType = XmlUtil::AttrToString(nodeElem, "projectionType", L"perspective");
	else
		projectionType = XmlUtil::AttrToString(nodeElem, "type", L"perspective");

	CameraEntity* camera = new CameraEntity();
	camera->Register();
	camera->SetName(name);
	camera->SetPosition(parentNode->_getDerivedPosition());
	camera->SetOrientation(parentNode->_getDerivedOrientation());
	camera->SetFOVy(Ogre::Radian(fov));
	camera->SetAspectRatio(aspectRatio);
	camera->SetCustomAspectRatio(aspectRatio > 0);


	if(projectionType == L"perspective")
		camera->SetProjectionType(CameraEntity::PROJECTION_PERSPECTIVE);
	else if(projectionType == L"orthographic")
		camera->SetProjectionType(CameraEntity::PROJECTION_ORTHOGRAPHIC);

	for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "clipping")
		{
			float nearDist = XmlUtil::AttrToFloat(elem, "near");
			camera->SetNearClipDistance(nearDist);

			float farDist = XmlUtil::AttrToFloat(elem, "far");
			camera->SetFarClipDistance(farDist);
		}
	}

	camera->PutToStage(m_Stage);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadLight(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode)
{
	WideString name = XmlUtil::AttrToString(nodeElem, "name");
	WideString id = XmlUtil::AttrToString(nodeElem, "id");

	LightEntity* lightEnt = new LightEntity();
	lightEnt->Register();
	lightEnt->SetName(name);
	lightEnt->SetPosition(parentNode->_getDerivedPosition());
	lightEnt->SetOrientation(parentNode->_getDerivedOrientation());	

	WideString type = XmlUtil::AttrToString(nodeElem, "type");
	if(type == L"point")
		lightEnt->SetLightType(LightEntity::LIGHT_POINT);
	else if(type == L"directional")
		lightEnt->SetLightType(LightEntity::LIGHT_DIRECTIONAL);
	else if(type == L"spot")
		lightEnt->SetLightType(LightEntity::LIGHT_SPOT);
	else if(type == L"radPoint")
		lightEnt->SetLightType(LightEntity::LIGHT_POINT);

	lightEnt->SetActive(XmlUtil::AttrToBool(nodeElem, "visible", true));
	lightEnt->SetCastShadows(XmlUtil::AttrToBool(nodeElem, "castShadows", true));


	for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "colourDiffuse")
		{
			lightEnt->SetDiffuseColor(GetColor(elem));
		}
		else if (elem->ValueStr() == "colourSpecular")
		{
			lightEnt->SetSpecularColor(GetColor(elem));
		}
		else if (elem->ValueStr() == "lightRange")
		{
			LoadLightRange(elem, lightEnt);
		}
		else if (elem->ValueStr() == "lightAttenuation")
		{
			LoadLightAttenuation(elem, lightEnt);
		}
	}

	lightEnt->PutToStage(m_Stage);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadLightRange(TiXmlElement* nodeElem, LightEntity* light)
{
	if (light->GetLightType() == LightEntity::LIGHT_SPOT)
	{
		if (nodeElem->Attribute("inner") != NULL)
			light->SetSpotlightInnerAngle(Ogre::Radian(XmlUtil::AttrToFloat(nodeElem, "inner")));

		if (nodeElem->Attribute("outer") != NULL)
			light->SetSpotlightOuterAngle(Ogre::Radian(XmlUtil::AttrToFloat(nodeElem, "outer")));

		if (nodeElem->Attribute("falloff") != NULL)
			light->SetSpotlightFalloff(XmlUtil::AttrToFloat(nodeElem, "falloff"));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadLightAttenuation(TiXmlElement* nodeElem, LightEntity* light)
{
	Ogre::Real range = nodeElem->Attribute("range") == NULL ? light->GetAttenuationRange() : XmlUtil::AttrToFloat(nodeElem, "range");

	Ogre::Real constant = nodeElem->Attribute("constant") == NULL ? light->GetAttenuationConstant() : XmlUtil::AttrToFloat(nodeElem, "constant");

	Ogre::Real linear = nodeElem->Attribute("linear") == NULL ? light->GetAttenuationLinear() : XmlUtil::AttrToFloat(nodeElem, "linear");

	Ogre::Real quadric = nodeElem->Attribute("quadric") == NULL ? light->GetAttenuationQuadratic() : XmlUtil::AttrToFloat(nodeElem, "quadric");

	light->SetAttenuation(range, constant, linear, quadric);
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadEnvironment(TiXmlElement* envElem)
{
	for (TiXmlElement* elem = envElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "colourAmbient")
		{
			Ogre::ColourValue colourValue = GetColor(elem);
			m_Stage->SetAmbientLightColor(colourValue);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadTrackTarget(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode)
{
	TrackTarget trackTarget(parentNode);
	trackTarget.NodeName = XmlUtil::AttrToString(nodeElem, "nodeName");

	for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "offset")
		{
			trackTarget.Offset = GetVector3(elem);
		}
		else if (elem->ValueStr() == "localDirection")
		{
			trackTarget.LocalDirection = GetVector3(elem);
		}
	}

	m_TrackTargets.push_back(trackTarget);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::LoadLookTarget(TiXmlElement* nodeElem, Ogre::SceneNode* parentNode)
{
	LookTarget lookTarget(parentNode);
	lookTarget.NodeName = XmlUtil::AttrToString(nodeElem, "nodeName");


	WideString relativeTo = XmlUtil::AttrToString(nodeElem, "relativeTo");
	if(relativeTo == L"local")
		lookTarget.RelativeTo = Ogre::Node::TS_LOCAL;
	else if(relativeTo == L"parent")
		lookTarget.RelativeTo = Ogre::Node::TS_PARENT;
	else if(relativeTo == L"world")
		lookTarget.RelativeTo = Ogre::Node::TS_WORLD;


	for (TiXmlElement* elem = nodeElem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "position")
		{
			lookTarget.Position = GetVector3(elem);
			lookTarget.HasPosition = true;
		}
		else if (elem->ValueStr() == "localDirection")
		{
			lookTarget.LocalDirection = GetVector3(elem);
		}
	}

	m_LookTargets.push_back(lookTarget);

	return true;
}



//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 OgreSceneLoader::GetVector3(TiXmlElement* elem)
{
	return Ogre::Vector3(
		XmlUtil::AttrToFloat(elem, "x"),
		XmlUtil::AttrToFloat(elem, "y"),
		XmlUtil::AttrToFloat(elem, "z")
		);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Quaternion OgreSceneLoader::GetRotation(TiXmlElement* elem)
{
	Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

	if (elem->Attribute("qx") != NULL)
	{
		// quaternion
		rotation.x = XmlUtil::AttrToFloat(elem, "qx");
		rotation.y = XmlUtil::AttrToFloat(elem, "qy");
		rotation.z = XmlUtil::AttrToFloat(elem, "qz");
		rotation.w = XmlUtil::AttrToFloat(elem, "qw");
	}
	else if (elem->Attribute("axisX") != NULL)
	{
		// axis and angle
		float angle = XmlUtil::AttrToFloat(elem, "angle");

		Ogre::Vector3 axis;
		axis.x = XmlUtil::AttrToFloat(elem, "axisX");
		axis.y = XmlUtil::AttrToFloat(elem, "axisY");
		axis.z = XmlUtil::AttrToFloat(elem, "axisZ");

		rotation.FromAngleAxis(Ogre::Radian(angle), axis);
	}
	else if (elem->Attribute("angleX") != NULL)
	{
		// Euler angles
		Ogre::Vector3 euler;
		euler.x = XmlUtil::AttrToFloat(elem, "angleX");
		euler.y = XmlUtil::AttrToFloat(elem, "angleY");
		euler.z = XmlUtil::AttrToFloat(elem, "angleZ");
		WideString order = XmlUtil::AttrToString(elem, "order");

		Ogre::Matrix3 rotationMatrix;
		if (order.length() < 2)
			rotationMatrix.FromEulerAnglesXYZ(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
		else
		{
			if (order[0] == L'x')
			{
				if (order[1] == L'y')
					rotationMatrix.FromEulerAnglesXYZ(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
				else
					rotationMatrix.FromEulerAnglesXZY(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
			}
			else if (order[0] == L'y')
			{
				if (order[1] == L'x')
					rotationMatrix.FromEulerAnglesYXZ(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
				else
					rotationMatrix.FromEulerAnglesYZX(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
			}
			else
			{
				if (order[1] == L'x')
					rotationMatrix.FromEulerAnglesZXY(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
				else
					rotationMatrix.FromEulerAnglesZYX(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
			}
		}
		rotation.FromRotationMatrix(rotationMatrix);
	}

	return rotation;
}

//////////////////////////////////////////////////////////////////////////
Ogre::ColourValue OgreSceneLoader::GetColor(TiXmlElement* elem)
{
	return Ogre::ColourValue(
		XmlUtil::AttrToFloat(elem, "r"),
		XmlUtil::AttrToFloat(elem, "g"),
		XmlUtil::AttrToFloat(elem, "b"),
		XmlUtil::AttrToFloat(elem, "a", 1.0)
		);
}

//////////////////////////////////////////////////////////////////////////
void OgreSceneLoader::ProcessLookTrackTargets()
{
	// process look targets
	foreach (LookTarget lookTarget, m_LookTargets)
	{
		Utf8String nodeName = StringUtil::WideToUtf8(lookTarget.NodeName);
		if (!m_Stage->GetSceneMgr()->hasSceneNode(nodeName)) continue;

		Ogre::SceneNode* sceneNode = m_Stage->GetSceneMgr()->getSceneNode(nodeName);

		Ogre::Vector3 position;
		if (lookTarget.HasPosition) position = lookTarget.Position;
		else
		{
			lookTarget.RelativeTo = Ogre::Node::TS_WORLD;
			position = sceneNode->_getDerivedPosition();
		}

		lookTarget.SourceNode->lookAt(position, lookTarget.RelativeTo, lookTarget.LocalDirection);
	}

	// process track targets
	foreach (TrackTarget trackTarget, m_TrackTargets)
	{
		Utf8String nodeName = StringUtil::WideToUtf8(trackTarget.NodeName);
		if (!m_Stage->GetSceneMgr()->hasSceneNode(nodeName)) continue;

		Ogre::SceneNode* sceneNode = m_Stage->GetSceneMgr()->getSceneNode(nodeName);
		trackTarget.SourceNode->setAutoTracking(true, sceneNode, trackTarget.LocalDirection, trackTarget.Offset);
	}

}

//////////////////////////////////////////////////////////////////////////
// Ogre::ScriptCompilerListener
//////////////////////////////////////////////////////////////////////////
bool OgreSceneLoader::handleEvent(Ogre::ScriptCompiler* compiler, Ogre::ScriptCompilerEvent* evt, void* retval)
{
	if (evt->mType == "processResourceName")
	{
		Ogre::ProcessResourceNameScriptCompilerEvent* nameEvt = static_cast<Ogre::ProcessResourceNameScriptCompilerEvent*>(evt);
		
		WideString texName = StringUtil::Utf8ToWide(nameEvt->mName);
		if (!Game::GetInstance()->GetFileMgr()->FileExists(texName))
		{
			WideString basePath = PathUtil::GetDirectoryName(m_FileName);

			texName = PathUtil::Combine(basePath, PathUtil::GetFileName(texName));
			nameEvt->mName = StringUtil::WideToUtf8(texName);
		}
		return true;
	}

	return false;
}


} // namespace Wme
