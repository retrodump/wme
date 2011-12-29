// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SceneNodeSortFilterModel.h"
#include "SceneNodeModel.h"
#include "SceneNode.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
SceneNodeSortFilterModel::SceneNodeSortFilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{

}

//////////////////////////////////////////////////////////////////////////
SceneNodeSortFilterModel::~SceneNodeSortFilterModel()
{

}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeSortFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	SceneNodeModel* model = qobject_cast<SceneNodeModel*>(sourceModel());
	if (!model) return true;

	QModelIndex index = model->index(sourceRow, 0, sourceParent);
	SceneNode* node = model->NodeForIndex(index);
	if (!node) return true;

	// filter system nodes
	if (node != model->GetRootNode())
	{
		QString nodeName = node->GetName();
		if (nodeName.startsWith("sys/", Qt::CaseInsensitive)) return false;
	}
	else return true;

	if (filterRegExp().isEmpty()) return true;
	
	QModelIndex nodeIndex = sourceModel()->index(sourceRow, 0, sourceParent);
	return MatchChildren(nodeIndex);
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeSortFilterModel::MatchChildren(const QModelIndex& parent) const
{
	QString name = sourceModel()->data(parent).toString();
	if (name.contains(filterRegExp())) return true;

	int numRows = sourceModel()->rowCount(parent);
	for (int i = 0; i < numRows; i++)
	{
		if (MatchChildren(parent.child(i, 0))) return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeSortFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	if (left.column() == SceneNodeModel::Name)
	{
		SceneNodeModel* model = qobject_cast<SceneNodeModel*>(sourceModel());
		if (!model) return false;
		
		SceneNode* leftNode = model->NodeForIndex(left);
		SceneNode* rightNode = model->NodeForIndex(right);

		// bones always come after other node types
		if (leftNode->GetType() == SceneNode::NODE_BONE && rightNode->GetType() != SceneNode::NODE_BONE) return false;
		else if (leftNode->GetType() != SceneNode::NODE_BONE && rightNode->GetType() == SceneNode::NODE_BONE) return true;

		return QString::localeAwareCompare(leftNode->GetName(), rightNode->GetName()) < 0;
	}
	else return QSortFilterProxyModel::lessThan(left, right);
}


} // namespace Armed
