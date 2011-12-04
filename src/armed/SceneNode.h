// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedSceneNode_H__
#define __ArmedSceneNode_H__

using namespace Wme;

namespace Wme
{
	class Scene3DBase;
	class Entity3DBase;
}



namespace Armed
{
	class SceneNode
	{
	public:
		SceneNode(Ogre::SceneNode* node, SceneNode* parent = NULL);
		SceneNode(Scene3DBase* scene, SceneNode* parent = NULL);
		SceneNode(Entity3DBase* entity, SceneNode* parent = NULL);
		SceneNode(Ogre::Bone* bone, SceneNode* parent = NULL);
		~SceneNode();

		enum NodeType
		{
			NODE_SCENE,
			NODE_GENERIC,
			NODE_ENTITY,
			NODE_BONE
		};

		SceneNode* GetParent() const { return m_Parent; }
		void SetParent(SceneNode* parent) { m_Parent = parent; }

		SceneNode* GetChildAt(int index) const;
		int GetRowOfChild(SceneNode* child) const { return m_Children.indexOf(child); }
		int GetNumChildren() const { return m_Children.count(); }
		bool HasChildren() const { return !m_Children.isEmpty(); }
		void AddChild(SceneNode* child);
		void RemoveChild(SceneNode* child);
		NodeType GetType() const { return m_Type; }

		QString GetName() const;
		QString GetTypeName() const;
		QIcon* GetIcon() const;

		Ogre::SceneNode* GetGenericNode() const { return m_GenericNode; }
		Entity3DBase* GetEntity() const { return m_Entity; }
		Scene3DBase* GetScene() const { return m_Scene; }
		Ogre::Bone* GetBone() const { return m_Bone; }

		Ogre::SceneNode* GetOgreSceneNode() const;
		SceneNode* GetChildFromOgreSceneNode(Ogre::SceneNode* ogreSceneNode) const;

	private:
		void GetEntityTypeAndIcon(Entity3DBase* entity, QString& typeName, QString& iconName) const;

		SceneNode* m_Parent;
		QList<SceneNode*> m_Children;
		QHash<Ogre::SceneNode*, SceneNode*> m_SceneNodeLookup;

		Scene3DBase* m_Scene;
		NodeType m_Type;
		Ogre::SceneNode* m_GenericNode;
		Entity3DBase* m_Entity;
		Ogre::Bone* m_Bone;
	};
}

#endif // __ArmedSceneNode_H__
