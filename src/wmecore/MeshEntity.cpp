// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MeshEntity.h"
#include "Scene3DBase.h"
#include "Exception.h"
#include "StringUtil.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "SimpleAnimTree.h"
#include "MousePickParams.h"
#include "MaterialManager.h"
#include "MaterialInstance.h"
#include "MaterialUtil.h"
#include "XmlUtil.h"
#include "NodeSelection.h"
#include "AttachmentPoint.h"
#include "OgreTagPoint.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MeshEntity::MeshEntity()
{
	m_OgreEntity = NULL;
	m_Mesh = NULL;
	m_AnimTree = NULL;

	m_HighlightMatAfterChange = false;

	m_CastShadows = true;
}

//////////////////////////////////////////////////////////////////////////
MeshEntity::~MeshEntity()
{
	RemoveFromStage();

	if (m_Mesh) Game::GetInstance()->GetMeshMgr()->ReleaseMesh(m_Mesh, this);

	
	foreach (MaterialInstanceMap::value_type mat, m_Materials)
	{
		Game::GetInstance()->GetMaterialMgr()->ReleaseMaterial(mat.second);
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::SetName(const WideString& name)
{
	if (name == m_Name) return;

	if (IsInStage()) m_Stage->OnRenameEntity(this, m_Name, name);
	m_Name = name;
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::SetCastShadows(bool val)
{
	m_CastShadows = val;
	if (IsInStage()) m_OgreEntity->setCastShadows(val);
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::KillAnimations()
{
	foreach (AnimationMap::value_type val, m_Animations)
	{
		SAFE_DELETE(val.second);
	}
	m_Animations.clear();
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::Update()
{
	if (!IsInStage()) return;

	UpdateTasks();

	if (m_AnimTree) m_AnimTree->Update();

	foreach (AttachmentMap::value_type val, m_Attachments)
	{
		val.second->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::Create(const WideString& meshFile, const WideString& skeletonFile)
{
	SetMesh(meshFile);
	if (m_Mesh) m_Mesh->SetSkeleton(skeletonFile);
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity)
{
	RemoveFromStage();

	if (m_Mesh)
	{
		Entity3DBase::PutToStage(stage, parentEntity);
		RebuildOgreEntity();
		m_Stage->OnAddEntity(this);
	}
	else
	{
		throw Exception(L"Cannot put 3D entity to stage - mesh doesn't exist.");
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RemoveFromStage()
{
	RemoveAllAttachments();

	Scene3DBase* stage = m_Stage;
	Entity3DBase::RemoveFromStage();
		
	KillAnimations();
	SAFE_DELETE(m_AnimTree);

	if (m_OgreEntity)
	{
		if (stage) stage->GetSceneMgr()->destroyEntity(m_OgreEntity);
		m_OgreEntity = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::BuildAnimTree()
{
	if (!m_AnimTree)
	{
		m_AnimTree = new SimpleAnimTree(this);
		m_AnimTree->Create();
	}
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::SetSkeleton(const WideString& skeletonFile, bool highlightMat)
{
	if (m_Mesh)
	{
		m_HighlightMatAfterChange = highlightMat;
		return m_Mesh->SetSkeleton(skeletonFile);
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::AddSkeleton(const WideString& skeletonFile, const WideString& animsToMerge)
{	
	if (m_Mesh)
	{
		ScheduleOgreEntityRebuild();
		return m_Mesh->AddSkeleton(skeletonFile, animsToMerge);
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RemoveSkeleton(bool highlightMat)
{
	if (m_Mesh)
	{
		m_HighlightMatAfterChange = highlightMat;
		m_Mesh->RemoveSkeleton();
	}
}

//////////////////////////////////////////////////////////////////////////
WideString MeshEntity::GetMeshFileName() const
{
	if (m_Mesh) return m_Mesh->GetFileName();
	else return L"";
}

//////////////////////////////////////////////////////////////////////////
WideString MeshEntity::GetSkeletonFileName() const
{
	if (m_Mesh) return m_Mesh->GetSkeletonFileName();
	else return L"";
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::SetMesh(const WideString& meshFile, bool highlightMat)
{
	m_HighlightMatAfterChange = highlightMat;

	RemoveMesh();

	m_Mesh = Game::GetInstance()->GetMeshMgr()->GetMesh(meshFile, this);

	if (m_Mesh)
	{
		if (IsInStage())
		{
			RebuildOgreEntity();

			// after changing the mesh Ogre will recreate sub entities during rendering
			// which will destroy material assignment
			// so we need to subscribe to the scene manager event and re-apply the materials
			ScheduleOgreEntityRebuild();
		}
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RemoveMesh()
{
	KillAnimations();
	SAFE_DELETE(m_AnimTree);

	if (m_OgreEntity)
	{
		if (m_Stage) m_Stage->GetSceneMgr()->destroyEntity(m_OgreEntity);
		m_OgreEntity = NULL;
	}

	if (m_Mesh)
	{
		Game::GetInstance()->GetMeshMgr()->ReleaseMesh(m_Mesh, this);
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RebuildOgreEntity()
{
	if (!IsInStage()) return;
	if (!m_Mesh) return;

	// make sure the name is conflict-free
	WideString internalName = m_Stage->GetUniqueMovableName("Entity", L"Entity");

	if (m_OgreEntity) m_Stage->GetSceneMgr()->destroyEntity(m_OgreEntity);
	m_OgreEntity = m_Stage->GetSceneMgr()->createEntity(StringUtil::WideToUtf8(internalName), m_Mesh->GetOgreMesh()->getName());
	m_OgreEntity->setListener(this);
	m_OgreEntity->setCastShadows(m_CastShadows);
	m_SceneNode->attachObject(m_OgreEntity);

	ReApplyAllMaterials();

	m_OgreEntity->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_ENTITY, this)));
	m_OgreEntity->getUserObjectBindings().setUserAny("MeshEntity", Ogre::Any(this));

	BuildAnimTree();
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::DeleteAnimation(const WideString& animationName)
{
	if (m_Mesh) return m_Mesh->DeleteAnimation(animationName);
	else return false;
}

//////////////////////////////////////////////////////////////////////////
Skeleton* MeshEntity::GetSkeleton() const
{
	if (m_Mesh) return m_Mesh->GetSkeleton();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::SetMaterial(const WideString& fileName, int subEntity)
{
	if (subEntity < 0)
	{
		if (m_OgreEntity)
		{
			for (size_t i = 0; i < m_OgreEntity->getNumSubEntities(); i++)
			{
				SetMaterial(fileName, i);
			}
		}
	}
	else
	{
		// release existing material
		MaterialInstanceMap::iterator it = m_Materials.find(subEntity);
		if (it != m_Materials.end())
		{
			if (m_Materials[subEntity]) Game::GetInstance()->GetMaterialMgr()->ReleaseMaterial(m_Materials[subEntity]);
		}

		// we are realized - set entity material
		if (m_OgreEntity)
		{
			if (subEntity >= 0 && subEntity < (int)m_OgreEntity->getNumSubEntities())
			{
				Ogre::SubEntity* sub = m_OgreEntity->getSubEntity(subEntity);
				bool isHighlighted = MaterialUtil::IsHighlightedName(sub->getMaterialName());
				m_Materials[subEntity] = Game::GetInstance()->GetMaterialMgr()->GetMaterial(fileName);

				ApplyMaterialInternal(subEntity, isHighlighted);
			}
		}
		// no entity yet, just store the material
		else m_Materials[subEntity] = Game::GetInstance()->GetMaterialMgr()->GetMaterial(fileName);
	}
}

//////////////////////////////////////////////////////////////////////////
MaterialInstance* MeshEntity::GetMaterial(size_t subEntity)
{
	MaterialInstanceMap::iterator it = m_Materials.find(subEntity);
	if (it == m_Materials.end()) return NULL;
	else return m_Materials[subEntity];
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::ReApplyAllMaterials()
{
	if (!m_Mesh) return;

	for (size_t i = 0; i < m_Mesh->GetOgreMesh()->getNumSubMeshes(); i++)
	{
		ReApplyMaterial(i);
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::ReApplyMaterial(size_t subEntity)
{
	if (!m_OgreEntity) return;

	Ogre::SubEntity* sub = m_OgreEntity->getSubEntity(subEntity);

	bool isHighlighted = MaterialUtil::IsHighlightedName(sub->getMaterialName());
	ApplyMaterialInternal(subEntity, isHighlighted);
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::ApplyMaterialInternal(size_t subEntity, bool highlighted)
{
	MaterialInstanceMap::iterator it = m_Materials.find(subEntity);
	if (it == m_Materials.end()) return;


	Ogre::SubEntity* sub = m_OgreEntity->getSubEntity(subEntity);

	if (highlighted)
	{
		Ogre::MaterialPtr mainMat;
		
		if (m_Materials[subEntity]) mainMat = m_Materials[subEntity]->GetOgreMaterial();
		else mainMat = MaterialUtil::GetEmptyMat();

		sub->setMaterial(mainMat); // necessary because of http://www.ogre3d.org/mantis/view.php?id=130
		Ogre::MaterialPtr hlMat = MaterialUtil::GetHighlightMat(mainMat, true);
		sub->setMaterial(hlMat);
	}
	else
	{
		if (m_Materials[subEntity]) sub->setMaterial(m_Materials[subEntity]->GetOgreMaterial());
		else sub->setMaterial(MaterialUtil::GetEmptyMat());
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::OnPreFindVisibleObjects()
{
	if (m_OgreEntity)
	{
		m_OgreEntity->_initialise(true);
		ReApplyAllMaterials();

		if (m_HighlightMatAfterChange) NodeSelection::HighlightEntity(m_OgreEntity, true);
		m_HighlightMatAfterChange = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::StartAnimation(const AnimProp& animProp)
{
	if (!m_AnimTree) return;

	switch (animProp.Mode)
	{
	case AnimProp::ANIM_START:
		m_AnimTree->StartAnimation(animProp.Layer, animProp.Name, false, animProp.FadeInTime, animProp.AnimWhileFadingIn, false);
		break;
	case AnimProp::ANIM_CROSSFADE:
	case AnimProp::ANIM_FORCE_CROSSFADE:
		if (animProp.Mode == AnimProp::ANIM_CROSSFADE && IsAnimationPlaying(animProp, true)) return;
		m_AnimTree->StartAnimation(animProp.Layer, animProp.Name, true, animProp.FadeInTime, animProp.AnimWhileFadingIn, animProp.AnimPrevWhileFadingIn);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::StopAnimation(const AnimProp& animProp)
{
	if (!m_AnimTree) return;
	m_AnimTree->StopAnimation(animProp.Name, animProp.FadeOutTime, animProp.AnimWhileFadingOut);
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::IsAnimationPlaying(const AnimProp& animProp, bool includeFadingIn)
{
	if (!m_AnimTree) return false;
	return m_AnimTree->IsAnimationPlaying(animProp.Layer, animProp.Name, includeFadingIn);
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::IsAnyAnimationPlaying()
{
	if (!m_AnimTree) return false;
	return m_AnimTree->IsAnyAnimationPlaying();
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::HasAnimation(const WideString& animName) const
{
	if (m_Mesh) return m_Mesh->HasAnimation(animName);
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::SupportsSkeletalAnimation() const
{
	return m_Mesh && m_Mesh->SupportsSkeletalAnimation();
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::OnSkeletonAdded()
{
	BuildAnimTree();	
	ScheduleOgreEntityRebuild();
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::OnSkeletonRemoved()
{
	KillAnimations();
	SAFE_DELETE(m_AnimTree);
	ScheduleOgreEntityRebuild();
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::ScheduleOgreEntityRebuild()
{
	if (IsInStage()) m_Stage->AddPreFindVisibleNotifyRequest(this);
}


//////////////////////////////////////////////////////////////////////////
void MeshEntity::AddAttachment(Entity3DBase* entity, const WideString& boneName)
{
	if (!IsInStage() || !SupportsSkeletalAnimation()) return;

	entity->DetachIfAttached();

	Utf8String boneNameUtf8 = StringUtil::WideToUtf8(boneName);
	if (!GetSkeleton()->GetOgreSkeleton()->hasBone(boneNameUtf8)) return;

	Ogre::Bone* bone = GetOgreEntity()->getSkeleton()->getBone(boneNameUtf8);
	AttachmentPoint* ap = new AttachmentPoint(entity, this, bone);
	m_Attachments.insert(AttachmentMap::value_type(boneName, ap));

	entity->SetAttachedTo(ap);
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RemoveAttachment(Entity3DBase* entity)
{
	for (AttachmentMap::iterator it = m_Attachments.begin(); it != m_Attachments.end(); ++it)
	{
		AttachmentPoint* ap = it->second;

		if (ap->GetAttachment() == entity)
		{
			ap->GetAttachment()->SetAttachedTo(NULL);
			delete ap;

			m_Attachments.erase(it);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RemoveAttachmentsFromBone(const WideString& boneName)
{
	std::pair<AttachmentMap::iterator, AttachmentMap::iterator> range;
	range = m_Attachments.equal_range(boneName);

	for (AttachmentMap::iterator it = range.first; it != range.second; ++it) 
	{
		AttachmentPoint* ap = it->second;
		ap->GetAttachment()->SetAttachedTo(NULL);
		delete ap;
	}

	m_Attachments.erase(range.first, range.second);
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::RemoveAllAttachments()
{
	foreach (AttachmentMap::value_type val, m_Attachments)
	{
		AttachmentPoint* ap = val.second;
		ap->GetAttachment()->SetAttachedTo(NULL);
		delete ap;
	}
	m_Attachments.clear();
}

//////////////////////////////////////////////////////////////////////////
void MeshEntity::GetAttachmentsForBone(const WideString& boneName, AttachmentList& attachments) const
{
	std::pair<AttachmentMap::const_iterator, AttachmentMap::const_iterator> range;
	range = m_Attachments.equal_range(boneName);

	for (AttachmentMap::const_iterator it = range.first; it != range.second; ++it) 
	{
		attachments.push_back(it->second);
	}
}


//////////////////////////////////////////////////////////////////////////
// Ogre::MovableObject::Listener
//////////////////////////////////////////////////////////////////////////
void MeshEntity::objectDestroyed(Ogre::MovableObject* obj)
{

}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::LoadFromXml(TiXmlElement* rootNode)
{
	Entity3DBase::LoadFromXml(rootNode);


	WideString meshFile;
	WideString skeletonFile;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Mesh")
		{
			for (TiXmlElement* meshElem = elem->FirstChildElement(); meshElem != NULL; meshElem = meshElem->NextSiblingElement())
			{
				if (meshElem->ValueStr() == "Mesh")
				{
					meshFile = XmlUtil::TextToString(meshElem);
				}
				else if (meshElem->ValueStr() == "Skeleton")
				{
					skeletonFile = XmlUtil::TextToString(meshElem);
				}
				else if (meshElem->ValueStr() == "Materials")
				{
					LoadMaterialsFromXml(meshElem);
				}
			}
		}
	}
	Create(meshFile, skeletonFile);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::SaveToXml(TiXmlElement* rootNode)
{
	Entity3DBase::SaveToXml(rootNode);

	TiXmlElement* meshElem = XmlUtil::AddElem("Mesh", rootNode);

	TiXmlElement* elem;

	if (m_Mesh)
	{
		elem = XmlUtil::AddElem("Mesh", meshElem);
		XmlUtil::SetFileName(elem, m_Mesh->GetFileName());

		if (m_Mesh->GetSkeletonFileName() != L"")
		{
			elem = XmlUtil::AddElem("Skeleton", meshElem);
			XmlUtil::SetFileName(elem, m_Mesh->GetSkeletonFileName());
		}
	}

	elem = XmlUtil::AddElem("Materials", meshElem);
	SaveMaterialsToXml(elem);


	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::LoadMaterialsFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Material")
		{
			size_t index = XmlUtil::AttrToSize(elem, "Index");
			WideString fileName = XmlUtil::TextToString(elem);

			SetMaterial(fileName, index);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::SaveMaterialsToXml(TiXmlElement* rootNode)
{
	foreach (MaterialInstanceMap::value_type mat, m_Materials)
	{
		if (mat.second == NULL) continue;

		TiXmlElement* elem = XmlUtil::AddElem("Material", rootNode);
		XmlUtil::SetFileName(elem, mat.second->GetFileName());
		XmlUtil::SetAttr(elem, "Index", mat.first);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void MeshEntity::RegisterScriptInterface()
{
	Entity::RegisterScriptInterface();

	RegisterScriptMethod(L"AddSkeleton",	(MethodHandler)&MeshEntity::ScAddSkeleton);
	RegisterScriptMethod(L"DeleteAnim",		(MethodHandler)&MeshEntity::ScDeleteAnim);
	RegisterScriptMethod(L"HasAnim",		(MethodHandler)&MeshEntity::ScHasAnim);
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::ScAddSkeleton(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	WideString skeletonFile = script->GetStack()->Pop()->GetString();
	WideString animList = script->GetStack()->Pop()->GetString(L"");
	bool ret = AddSkeleton(skeletonFile, animList);

	script->GetStack()->Push(ret);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::ScDeleteAnim(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString animName = script->GetStack()->Pop()->GetString();
	bool ret = DeleteAnimation(animName);

	script->GetStack()->Push(ret);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MeshEntity::ScHasAnim(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString animName = script->GetStack()->Pop()->GetString();

	script->GetStack()->Push(HasAnimation(animName));
	return true;
}


} // namespace Wme
