// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SceneNodeModel.h"
#include "SceneNode.h"
#include "DocScene.h"
#include "Scene3DBase.h"
#include "Entity3DBase.h"
#include "MeshEntity.h"
#include "QtUtil.h"
#include "Skeleton.h"
#include "StringUtil.h"


namespace Armed
{


const QString SceneNodeModel::MimeType = "application/vnd.wme.armitage.scenenodes";

//////////////////////////////////////////////////////////////////////////
SceneNodeModel::SceneNodeModel(QObject* parent) : QAbstractItemModel(parent)
{
	m_Scene = NULL;
	m_RootNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
SceneNodeModel::~SceneNodeModel()
{
	if (m_RootNode) delete m_RootNode;
	if (m_Scene) m_Scene->GetScene()->RemoveSceneGraphListener(this);
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::SetScene(DocScene* scene)
{
	if (m_Scene) m_Scene->GetScene()->RemoveSceneGraphListener(this);
	m_Scene = scene;
	BuildSceneTree();
	if (m_Scene) m_Scene->GetScene()->AddSceneGraphListener(this);

	reset();
}


//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::BuildSceneTree()
{
	if (m_RootNode) delete m_RootNode;

	m_RootNode = new SceneNode(this, (Ogre::SceneNode*)NULL);
	if (m_Scene)
	{
		SceneNode* scene = new SceneNode(this, m_Scene->GetScene(), m_RootNode);
		PopulateNode(scene, m_Scene->GetScene()->GetRootNode());
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::PopulateNode(SceneNode* parentNode, Ogre::SceneNode* ogreSceneNode)
{
	for (unsigned short i = 0; i < ogreSceneNode->numChildren(); i++)
	{
		SceneNode* newNode;
		Ogre::SceneNode* childSceneNode = static_cast<Ogre::SceneNode*>(ogreSceneNode->getChild(i));

		Entity3DBase* entity = Entity3DBase::NodeToEntity(childSceneNode);
		if (entity)
		{
			newNode = new SceneNode(this, entity, parentNode);

			// bones
			if (entity->IsMeshBased())
			{
				MeshEntity* meshEntity = static_cast<MeshEntity*>(entity);

				Skeleton* skeleton = meshEntity->GetSkeleton();
				if (skeleton)
				{
					Ogre::SkeletonPtr ogreSkeleton = skeleton->GetOgreSkeleton();
					Ogre::Skeleton::BoneIterator boneIter = ogreSkeleton->getRootBoneIterator();
					while (boneIter.hasMoreElements())
					{
						PopulateNode(newNode, meshEntity, boneIter.getNext());
					}
				}
			}
		}
		else
		{
			newNode = new SceneNode(this, childSceneNode, parentNode);
		}
		PopulateNode(newNode, childSceneNode);
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::PopulateNode(SceneNode* parentNode, MeshEntity* meshEntity, Ogre::Bone* bone)
{
	SceneNode* newNode = new SceneNode(this, meshEntity, bone, parentNode);

	// attachments
	AttachmentList attachments;
	meshEntity->GetAttachmentsForBone(StringUtil::Utf8ToWide(bone->getName()), attachments);

	foreach (AttachmentPoint* ap, attachments)
	{
		PopulateNode(newNode, ap->GetNode());
	}


	// child bones
	for (unsigned short i = 0; i < bone->numChildren(); i++)
	{
		PopulateNode(newNode, meshEntity, static_cast<Ogre::Bone*>(bone->getChild(i)));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::RegisterNode(SceneNode* node, Ogre::SceneNode* ogreNode)
{
	if (!ogreNode) return;

	m_NodeMap[ogreNode] = node;
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::UnregisterNode(Ogre::SceneNode* ogreNode)
{
	m_NodeMap.remove(ogreNode);
}

//////////////////////////////////////////////////////////////////////////
Ogre::SceneNode* SceneNodeModel::GetRealRootNode() const
{
	if (!m_Scene || !m_Scene->GetScene() || !m_Scene->GetScene()->GetSceneMgr()) return NULL;

	return m_Scene->GetScene()->GetSceneMgr()->getRootSceneNode();
}

//////////////////////////////////////////////////////////////////////////
SceneNode* SceneNodeModel::NodeForIndex(const QModelIndex& index) const
{
	if (index.isValid())
	{
		if (SceneNode* node = static_cast<SceneNode*>(index.internalPointer()))
			return node;
	}
	return m_RootNode;
}

//////////////////////////////////////////////////////////////////////////
QModelIndex SceneNodeModel::IndexFromNode(SceneNode* node) const
{	
	if (node)
	{
		SceneNode* parentNode = node->GetParent();
		if (parentNode)
		{
			int row = parentNode->GetRowOfChild(node);
			return createIndex(row, 0, node);
		}
	}
	return QModelIndex();
}

//////////////////////////////////////////////////////////////////////////
QModelIndex SceneNodeModel::IndexFromNode(Ogre::SceneNode* node) const
{
	if (!node || !m_NodeMap.contains(node)) return QModelIndex();
	else return IndexFromNode(m_NodeMap[node]);
}

//////////////////////////////////////////////////////////////////////////
// QAbstractItemModel
//////////////////////////////////////////////////////////////////////////
Qt::ItemFlags SceneNodeModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
	if (index.isValid())
	{
		theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		if (index.column() == Name)
		{
			theFlags |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
			
			if (NodeForIndex(index)->GetType() != SceneNode::NODE_SCENE && NodeForIndex(index)->GetType() != SceneNode::NODE_BONE)
				theFlags |= Qt::ItemIsDragEnabled;
		}
	}
	return theFlags;
}

//////////////////////////////////////////////////////////////////////////
QVariant SceneNodeModel::data(const QModelIndex& index, int role) const
{
	if (!GetRealRootNode() || !index.isValid() || index.column() < 0 || index.column() >= ColumnCount)
		return QVariant();
	
	if (SceneNode* node = NodeForIndex(index))
	{
		if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
		{
			switch (index.column())
			{
			case Name:
				if (role == Qt::ToolTipRole) return QString(tr("%1 (%2)")).arg(node->GetName()).arg(node->GetTypeName());
				else return node->GetName();
			case Type:
				return node->GetTypeName();
			default:
				Q_ASSERT(false);
			}
		}
		
		if (role == Qt::TextAlignmentRole)
		{
			return static_cast<int>(Qt::AlignVCenter | Qt::AlignLeft);
		}
		
		if (role == Qt::DecorationRole && index.column() == Name)
		{
			QIcon* icon = node->GetIcon();

			if (icon) return *icon;
			else return QVariant();
		}
	}
	return QVariant();
}

//////////////////////////////////////////////////////////////////////////
QVariant SceneNodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == Name)
			return tr("Name");
		else if (section == Type)
			return tr("Type");
	}
	return QVariant();
}

//////////////////////////////////////////////////////////////////////////
int SceneNodeModel::rowCount(const QModelIndex& parent) const
{
	if (!m_Scene) return 0;

	if (parent.isValid() && parent.column() != 0)
		return 0;
	
	SceneNode* parentNode = NodeForIndex(parent);
	return parentNode ? parentNode->GetNumChildren() : 0;
}

//////////////////////////////////////////////////////////////////////////
int SceneNodeModel::columnCount(const QModelIndex& parent) const
{
	return parent.isValid() && parent.column() != 0 ? 0 : ColumnCount;
}

//////////////////////////////////////////////////////////////////////////
QModelIndex SceneNodeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!GetRealRootNode() || row < 0 || column < 0 || column >= ColumnCount || (parent.isValid() && parent.column() != 0))
		return QModelIndex();

	SceneNode* parentNode = NodeForIndex(parent);
	if (SceneNode* node = parentNode->GetChildAt(row))
		return createIndex(row, column, node);
	
	return QModelIndex();
}

//////////////////////////////////////////////////////////////////////////
QModelIndex SceneNodeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) return QModelIndex();

	if (SceneNode* childNode = NodeForIndex(index))
	{
		if (SceneNode* parentNode = childNode->GetParent())
		{
			if (parentNode == GetRootNode())
				return QModelIndex();

			if (SceneNode* grandParentNode = parentNode->GetParent())
			{
				int row = grandParentNode->GetRowOfChild(parentNode);
				return createIndex(row, 0, parentNode);
			}
		}
	}
	return QModelIndex();
}

//////////////////////////////////////////////////////////////////////////
QStringList SceneNodeModel::mimeTypes() const
{
	QStringList types;
	types << MimeType;
	return types;
}

//////////////////////////////////////////////////////////////////////////
QMimeData* SceneNodeModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();

	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	qforeach (QModelIndex index, indexes)
	{
		if (index.isValid())
		{
			Ogre::SceneNode* sceneNode = NodeForIndex(index)->GetOgreSceneNode();
			if (sceneNode) stream << QString::fromUtf8(sceneNode->getName().c_str());
		}
	}

	mimeData->setData(MimeType, encodedData);

	return mimeData;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{

	if (action == Qt::IgnoreAction)
		return true;
	
	if (action != Qt::MoveAction || column > 0 || !mimeData || !mimeData->hasFormat(MimeType))
		return false;

	SceneNode* parentNode = NodeForIndex(parent);
	if (!parentNode) return false;

	Ogre::SceneNode* parentSceneNode = parentNode->GetOgreSceneNode();
	if (!parentSceneNode && parentNode->GetType() != SceneNode::NODE_BONE) return false;


	Ogre::SceneManager* sceneMgr = m_Scene->GetScene()->GetSceneMgr();

	QByteArray encodedData = mimeData->data(MimeType);
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QStringList nodeList;

	while (!stream.atEnd())
	{
		QString nodeName;
		stream >> nodeName;
		nodeList << nodeName;
	}

	m_DroppedNodes.clear();	
	qforeach (QString nodeName, nodeList)
	{
		try
		{
			bool unparent = false;

			Ogre::SceneNode* childSceneNode = sceneMgr->getSceneNode(nodeName.toUtf8().constData());
			SceneNode* childNode = NodeForIndex(IndexFromNode(childSceneNode));

			// dropped to our own parent -> means the user wants to go one lever higher in the hierarchy
			if (childSceneNode->getParentSceneNode() == parentSceneNode)
			{
				// if our parent is the scene we can't go any higher, so ignore
				if (parentNode->GetParent() == GetRootNode())
				{
					m_DroppedNodes.append(childNode);
					continue;
				}
				else unparent = true;
			}

			Ogre::SceneNode* targetSceneNode = NULL;
			SceneNode* targetNode = NULL;
			if (parentSceneNode)
			{
				targetSceneNode = unparent ? parentSceneNode->getParentSceneNode() : parentSceneNode;			
				targetNode = NodeForIndex(IndexFromNode(targetSceneNode));
			}
			else
			{
				targetSceneNode = NULL;
				targetNode = parentNode;
			}

			// bones can only accept entities
			if (targetNode->GetType() == SceneNode::NODE_BONE && childNode->GetType() != SceneNode::NODE_ENTITY)
			{
				m_DroppedNodes.append(childNode);
				continue;
			}


			int oldPos = childNode->GetParent()->GetRowOfChild(childNode);
			int newPos = targetNode->GetNumChildren();

			beginMoveRows(IndexFromNode(childNode->GetParent()), oldPos, oldPos, IndexFromNode(targetNode), newPos);

			// remember the world position so that we can restore it
			Ogre::Vector3 pos = childSceneNode->_getDerivedPosition();
			Ogre::Quaternion rot = childSceneNode->_getDerivedOrientation();

			// move the node to a new parent

			// is the old parent a bone?
			SceneNode* oldParent = childNode->GetParent();
			if (oldParent && oldParent->GetType() == SceneNode::NODE_BONE && childNode->GetType() == SceneNode::NODE_ENTITY)
			{
				oldParent->GetBoneOwner()->RemoveAttachment(childNode->GetEntity());
			}
			else
			{
				childSceneNode->getParentSceneNode()->removeChild(childSceneNode);
			}


			if (targetSceneNode)
			{
				// target is a scene node
				targetSceneNode->addChild(childSceneNode);

				// restore original world position
				childSceneNode->_setDerivedPosition(pos);
				childSceneNode->_setDerivedOrientation(rot);
			}
			else
			{
				// target is a bone
				WideString boneName = StringUtil::Utf8ToWide(targetNode->GetBone()->getName());
				targetNode->GetBoneOwner()->AddAttachment(childNode->GetEntity(), boneName);
			}
			

			// project changes to scene model
			childNode->GetParent()->RemoveChild(childNode);
			targetNode->AddChild(childNode);
						
			endMoveRows();

			m_DroppedNodes.append(childNode);
		}
		catch (Ogre::Exception e)
		{
			continue;
		}
	}

	// refresh gizmos and data display
	m_Scene->RefreshEditor();

	// emit event after the drag&drop is over (QTreeView won't auto-expand selection parents if d&d is in progress)
	QTimer::singleShot(0, this, SLOT(SelectItemsAfterDrop()));

	return true;
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::SelectItemsAfterDrop()
{
	QModelIndexList droppedIndexes;
	qforeach (SceneNode* node, m_DroppedNodes)
	{
		droppedIndexes.append(IndexFromNode(node));
	}

	emit IndexesDropped(droppedIndexes);
	m_DroppedNodes.clear();
}

//////////////////////////////////////////////////////////////////////////
// Wme::Scene3DBase::SceneGraphListener
//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::OnSceneNodeAdded(Ogre::SceneNode* node)
{
	Ogre::SceneNode* parentSceneNode = node->getParentSceneNode();
	if (!parentSceneNode) return;

	QModelIndex parentIndex = IndexFromNode(parentSceneNode);
	SceneNode* parentNode = NodeForIndex(parentIndex);
	if (!parentNode) return;

	SceneNode* newNode = new SceneNode(this, node, parentNode);
	int row = parentNode->GetRowOfChild(newNode);

	beginInsertRows(parentIndex, row, row);
	endInsertRows();

	emit dataChanged(IndexFromNode(newNode), IndexFromNode(newNode));
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeModel::OnSceneNodeRemoving(Ogre::SceneNode* node)
{
	QModelIndex nodeIndex = IndexFromNode(node);
	if (!nodeIndex.isValid()) return;
	SceneNode* sceneNode = NodeForIndex(nodeIndex);

	QModelIndex parentIndex = parent(nodeIndex);
	SceneNode* parentNode = NodeForIndex(parentIndex);

	int row = parentNode->GetRowOfChild(sceneNode);

	beginRemoveRows(parentIndex, row, row);
	endRemoveRows();
}



} // namespace Armed
