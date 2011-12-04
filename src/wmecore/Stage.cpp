// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Renderer.h"
#include "Stage.h"
#include "View.h"
#include "Viewport.h"
#include "Camera.h"
#include "HybridCamera.h"
#include "Entity.h"
#include "StringUtil.h"
#include "OgreUtil.h"
#include "MathUtil.h"
#include "StageEditor.h"
#include "NavPath.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Stage::Stage()
{
	m_SceneMgr = NULL;
	m_RaySceneQuery = NULL;
	m_Default2DCamera = NULL;
	m_Editor = NULL;
}

//////////////////////////////////////////////////////////////////////////
Stage::~Stage()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////
void Stage::Create()
{
	Destroy();

	m_SceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_SceneMgr->addRenderQueueListener(this);
	m_SceneMgr->addListener(this);

	m_RootNode = m_SceneMgr->getRootSceneNode()->createChildSceneNode("sys/root_node");
	m_SystemRootNode = m_SceneMgr->getRootSceneNode()->createChildSceneNode("sys/system_root_node");

	m_RaySceneQuery = m_SceneMgr->createRayQuery(Ogre::Ray(), Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
	m_RaySceneQuery->setSortByDistance(true); 

	m_Default2DCamera = new Camera;
	m_Default2DCamera->Create(this, L"Default2DCamera");
}

//////////////////////////////////////////////////////////////////////////
void Stage::Destroy()
{
	SAFE_DELETE(m_Editor);
	SAFE_DELETE(m_Default2DCamera);

	foreach (Camera* camera, m_Cameras)
	{
		SAFE_DELETE(camera);
	}
	m_Cameras.clear();


	if (m_SceneMgr)
	{
		m_SceneMgr->removeListener(this);
		m_SceneMgr->removeRenderQueueListener(this);

		if (m_RaySceneQuery) m_SceneMgr->destroyQuery(m_RaySceneQuery);
		m_RaySceneQuery = NULL;

		Ogre::Root::getSingleton().destroySceneManager(m_SceneMgr);
		m_SceneMgr = NULL;
	}

	foreach (EntityTypeMap::value_type& typeMapPair, m_NameTypeMap)
	{
		SAFE_DELETE(typeMapPair.second);
	}
	m_NameTypeMap.clear();
}

//////////////////////////////////////////////////////////////////////////
void Stage::Shutdown()
{
	ShutdownScripts();
}

//////////////////////////////////////////////////////////////////////////
Ogre::SceneNode* Stage::GetRootNode() const
{
	if (!m_SceneMgr) return NULL;

	if (m_RootNode) return m_RootNode;
	else return m_SceneMgr->getRootSceneNode();
}

//////////////////////////////////////////////////////////////////////////
Ogre::SceneNode* Stage::GetSystemRootNode() const
{
	if (!m_SceneMgr) return NULL;

	if (m_SystemRootNode) return m_SystemRootNode;
	else return m_SceneMgr->getRootSceneNode();
}

//////////////////////////////////////////////////////////////////////////
void Stage::Update()
{
	if (m_Editor) m_Editor->Update();

	foreach (Entity* entity, m_ContainedEntities)
	{
		entity->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
void Stage::Render(Viewport* viewport, Camera* camera)
{
	foreach (Entity* entity, m_ContainedEntities)
	{
		entity->PreRender(viewport);
	}
	if (m_Editor) m_Editor->PreRender(viewport);
}

//////////////////////////////////////////////////////////////////////////
Camera* Stage::AddCamera(const WideString& name)
{
	if (!m_SceneMgr) return NULL;

	Ogre::Camera* ogreCamera = m_SceneMgr->getCamera(StringUtil::WideToUtf8(name));
	if (!ogreCamera) return NULL;

	Camera* camera = new Camera();
	camera->CreateFromOgreCamera(this, ogreCamera);

	m_Cameras.push_back(camera);

	return camera;
}

//////////////////////////////////////////////////////////////////////////
HybridCamera* Stage::AddHybridCamera(const WideString& name)
{
	if (!m_SceneMgr) return NULL;

	Utf8String utf8Name = StringUtil::WideToUtf8(name);

	if (!m_SceneMgr->hasCamera(utf8Name)) return NULL;

	Ogre::Camera* ogreCamera = m_SceneMgr->getCamera(utf8Name);

	HybridCamera* camera = new HybridCamera();
	camera->CreateFromOgreCamera(this, ogreCamera);

	m_Cameras.push_back(camera);

	return camera;
}

//////////////////////////////////////////////////////////////////////////
void Stage::RemoveCamera(Camera* camera)
{
	m_Cameras.remove(camera);
	SAFE_DELETE(camera);
}

//////////////////////////////////////////////////////////////////////////
void Stage::_DestroyOgreCamera(Camera* camera)
{
	if (camera && camera->GetOgreCamera() && m_SceneMgr) m_SceneMgr->destroyCamera(camera->GetOgreCamera());
}

//////////////////////////////////////////////////////////////////////////
bool Stage::RaycastFromPoint(MousePickParams& pickParams)
{
	// create the ray to test
	Ogre::Ray ray(Ogre::Vector3(pickParams.Point.x, pickParams.Point.y, pickParams.Point.z),
		Ogre::Vector3(pickParams.Normal.x, pickParams.Normal.y, pickParams.Normal.z));

	// check we are initialised
	if (m_RaySceneQuery != NULL)
	{
		// create a query object
		m_RaySceneQuery->setRay(ray);

		// execute the query, returns a vector of hits
		if (m_RaySceneQuery->execute().size() <= 0)
		{
			// raycast did not hit an objects bounding box
			return (false);
		}
	}
	else
	{
		Game::GetInstance()->Log(L"Cannot raycast without RaySceneQuery instance");
		return (false);
	}   

	// at this point we have raycast to a series of different objects bounding boxes.
	// we need to test these different objects to see which is the first polygon hit.
	// there are some minor optimizations (distance based) that mean we wont have to
	// check all of the objects most of the time, but the worst case scenario is that
	// we need to test every triangle of every object.
	Ogre::Real closest_distance = -1.0f;
	Ogre::Vector3 closest_result;
	Ogre::Vector2 closestUV[3];
	Ogre::Vector3 closestTri[3];
	unsigned short closest_sub;

	Ogre::RaySceneQueryResult& query_result = m_RaySceneQuery->getLastResults();
	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		// stop checking if we have found a raycast hit that is closer
		// than all remaining entities
		if ((closest_distance >= 0.0f) &&
			(closest_distance < query_result[qr_idx].distance))
		{
			break;
		}

		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL) &&
			(query_result[qr_idx].movable->getMovableType().compare("Entity") == 0) && query_result[qr_idx].movable->isVisible() &&
			(pickParams.RenderQueueId < 0 || query_result[qr_idx].movable->getRenderQueueGroup() == pickParams.RenderQueueId))
		{
			// get the entity to check
			Ogre::Entity* pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);


			// mesh data to retrieve         
			size_t vertex_count;
			size_t index_count;
			Ogre::Vector3* vertices;
			unsigned short* sub_indices;
			Ogre::Vector2* uv_coords;
			unsigned long* indices;			

			// get the mesh information
			OgreUtil::GetMeshInformation(pentity, pickParams.QueryUV, vertex_count, vertices, sub_indices, uv_coords, index_count, indices);

			// test for hitting individual triangles on the mesh
			bool new_closest_found = false;
			for (int i = 0; i < static_cast<int>(index_count); i += 3)
			{
				// check for a hit against this triangle
				std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
					vertices[indices[i+1]], vertices[indices[i+2]], true, false);

				// if it was a hit check if its the closest
				if (hit.first)
				{
					if ((closest_distance < 0.0f) || (hit.second < closest_distance))
					{
						// this is the closest so far, save it off
						closest_distance = hit.second;
						new_closest_found = true;
						pickParams.ResultEntity = pentity;
						closest_sub = sub_indices[indices[i]];

						if (pickParams.QueryUV)
						{
							for (int j = 0; j < 3; j++)
							{
								closestTri[j] = vertices[indices[i + j]];
								closestUV[j] = uv_coords[indices[i + j]];
							}
						}
					}
				}
			}

			// free the verticies and indicies memory
			SAFE_DELETE_ARRAY(vertices);
			SAFE_DELETE_ARRAY(sub_indices);
			SAFE_DELETE_ARRAY(uv_coords);
			SAFE_DELETE_ARRAY(indices);

			// if we found a new closest raycast for this object, update the
			// closest_result before moving on to the next object.
			if (new_closest_found)
			{
				closest_result = ray.getPoint(closest_distance);               
			}
		}       
	}

	// return the result
	if (closest_distance >= 0.0f)
	{
		// raycast success
		pickParams.ResultPoint = closest_result;

		pickParams.ResultSubEntity = pickParams.ResultEntity->getSubEntity(closest_sub);

		// calculate uv coordinates
		if (pickParams.QueryUV)
		{
			Ogre::Vector3 bc = MathUtil::GetBarycentricCoordinates(closestTri[0], closestTri[1], closestTri[2], pickParams.ResultPoint);
			pickParams.ResultUV = closestUV[0] * bc.x + closestUV[1] * bc.y + closestUV[2] * bc.z;
		}
		else pickParams.ResultUV = Ogre::Vector2::ZERO;

		return true;
	}
	else
	{
		// raycast failed
		pickParams.ResultEntity = NULL;
		pickParams.ResultSubEntity = NULL;
		return false;
	} 
}

//////////////////////////////////////////////////////////////////////////
void Stage::OnAddEntity(Entity* entity)
{
	if (!entity) return;

	if (m_ContainedEntities.find(entity) == m_ContainedEntities.end())
		m_ContainedEntities.insert(entity);


	WideString subType = entity->GetEntitySubtype();

	EntityNameMap* nameMap;

	if (m_NameTypeMap.count(subType) == 0)
	{
		nameMap = new EntityNameMap();
		m_NameTypeMap[subType] = nameMap;
	}
	else
	{
		nameMap = m_NameTypeMap[subType];
	}
	nameMap->insert(EntityNameMap::value_type(entity->GetName(), entity));
}

//////////////////////////////////////////////////////////////////////////
void Stage::OnRemoveEntity(Entity* entity)
{
	if (!entity) return;

	if (m_ContainedEntities.find(entity) != m_ContainedEntities.end())
		m_ContainedEntities.erase(entity);

	WideString subType = entity->GetEntitySubtype();
	if (m_NameTypeMap.count(subType) > 0)
	{
		EntityNameMap* nameMap = m_NameTypeMap[subType];
		EntityNameMap::iterator from = nameMap->lower_bound(entity->GetName());
		EntityNameMap::iterator to = nameMap->upper_bound(entity->GetName());

		for (EntityNameMap::iterator it = from; it != to; ++it)
		{
			if ((*it).second == entity)
			{
				nameMap->erase(it);
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Stage::OnRenameEntity(Entity* entity, const WideString& oldName, const WideString& newName)
{
	if (!entity) return;
	if (oldName == newName) return;

	WideString subType = entity->GetEntitySubtype();
	if (m_NameTypeMap.count(subType) > 0)
	{
		EntityNameMap* nameMap = m_NameTypeMap[subType];
		EntityNameMap::iterator from = nameMap->lower_bound(oldName);
		EntityNameMap::iterator to = nameMap->upper_bound(oldName);

		// remove the old value
		for (EntityNameMap::iterator it = from; it != to; ++it)
		{
			if ((*it).second == entity)
			{
				nameMap->erase(it);
				break;
			}
		}

		// add the new value
		nameMap->insert(EntityNameMap::value_type(newName, entity));
	}
}

//////////////////////////////////////////////////////////////////////////
void Stage::GetEntitiesByName(const WideString& subType, const WideString& entityName, Entity::EntityList& entityList)
{
	if (m_NameTypeMap.count(subType) == 0) return;

	EntityNameMap* nameMap = m_NameTypeMap[subType];
	EntityNameMap::iterator from = nameMap->lower_bound(entityName);
	EntityNameMap::iterator to = nameMap->upper_bound(entityName);

	for (EntityNameMap::iterator it = from; it != to; ++it)
	{
		entityList.push_back((*it).second);
	}
}

//////////////////////////////////////////////////////////////////////////
Entity* Stage::GetEntityByName(const WideString& subType, const WideString& entityName)
{
	Entity::EntityList entList;
	GetEntitiesByName(subType, entityName, entList);
	if (!entList.empty()) return entList.front();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void Stage::SetEditor(StageEditor* editor)
{
	if (editor == m_Editor) return;

	SAFE_DELETE(m_Editor);
	m_Editor = editor;
}

//////////////////////////////////////////////////////////////////////////
void Stage::GetPath(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, NavPath* path)
{
	path->Reset();
	path->SetReady(true);
}

//////////////////////////////////////////////////////////////////////////
bool Stage::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	return InvokeScriptMouseEvent(viewport, event);
}

//////////////////////////////////////////////////////////////////////////
WideString Stage::GetUniqueNodeName(const WideString& origName)
{
	if (!m_SceneMgr) return origName;
	if (!m_SceneMgr->hasSceneNode(StringUtil::WideToUtf8(origName))) return origName;

	Utf8String baseName = StringUtil::WideToUtf8(origName);
	int counter = 2;

	while (true)
	{		
		Utf8String newName = baseName + Ogre::StringConverter::toString(counter);
		if (!m_SceneMgr->hasSceneNode(newName)) return StringUtil::Utf8ToWide(newName);
		counter++;
	}
}

//////////////////////////////////////////////////////////////////////////
WideString Stage::GetUniqueMovableName(const Utf8String& movableType, const WideString& origName)
{
	if (!m_SceneMgr) return origName;
	if (!m_SceneMgr->hasMovableObject(StringUtil::WideToUtf8(origName), movableType)) return origName;

	Utf8String baseName = StringUtil::WideToUtf8(origName);
	int counter = 2;

	while (true)
	{		
		Utf8String newName = baseName + Ogre::StringConverter::toString(counter);
		if (!m_SceneMgr->hasMovableObject(newName, movableType)) return StringUtil::Utf8ToWide(newName);
		counter++;
	}
}

//////////////////////////////////////////////////////////////////////////
void Stage::AddPreFindVisibleNotifyRequest(Entity* ent)
{
	m_PreFindVisibleNotifyRequests.insert(ent);
}

//////////////////////////////////////////////////////////////////////////
void Stage::AddPostFindVisibleNotifyRequest(Entity* ent)
{
	m_PostFindVisibleNotifyRequests.insert(ent);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::RenderQueueListener implementation
//////////////////////////////////////////////////////////////////////////
void Stage::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation)
{
	Game::GetInstance()->GetRenderer()->OnRenderQueueStart(queueGroupId, m_SceneMgr->getCurrentViewport());
}

//////////////////////////////////////////////////////////////////////////
void Stage::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{
	// nothing
}

//////////////////////////////////////////////////////////////////////////
// Ogre::SceneManager::Listener implementation
//////////////////////////////////////////////////////////////////////////
void Stage::preFindVisibleObjects(Ogre::SceneManager* source, Ogre::SceneManager::IlluminationRenderStage irs, Ogre::Viewport* v)
{
	foreach (Entity* ent, m_PreFindVisibleNotifyRequests)
	{
		ent->OnPreFindVisibleObjects();
	}
	m_PreFindVisibleNotifyRequests.clear();
}

//////////////////////////////////////////////////////////////////////////
void Stage::postFindVisibleObjects(Ogre::SceneManager* source, Ogre::SceneManager::IlluminationRenderStage irs, Ogre::Viewport* v)
{
	foreach (Entity* ent, m_PostFindVisibleNotifyRequests)
	{
		ent->OnPostFindVisibleObjects();
	}
	m_PostFindVisibleNotifyRequests.clear();
}


} // namespace Wme
