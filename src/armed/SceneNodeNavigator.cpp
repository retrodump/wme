// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SceneNodeNavigator.h"
#include "SceneNodeModel.h"
#include "SceneNode.h"
#include "SceneNodeSortFilterModel.h"
#include "DocScene.h"
#include "FilterWidget.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
SceneNodeNavigator::SceneNodeNavigator(QWidget* parent, DocScene* docScene) : QWidget(parent)
{
	m_IgnoreSelChanges = false;

	m_FilterWidget = new FilterWidget(this);

	m_DocScene = docScene;


	m_SceneTree = new QTreeView(this);
	m_SceneTree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_SceneTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_SceneTree->setAllColumnsShowFocus(true);
	m_SceneTree->setDragDropMode(QAbstractItemView::InternalMove);
	m_SceneTree->header()->setMovable(false);
	//m_SceneTree->setAlternatingRowColors(true);

	m_SortFilterModel = new SceneNodeSortFilterModel(this);
	m_SortFilterModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_SortFilterModel->setSourceModel(m_DocScene->GetSceneNodeModel());
	
	m_SceneTree->setModel(m_SortFilterModel);
	m_SceneTree->setSortingEnabled(true);


	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);	
	layout->addWidget(m_FilterWidget);
	layout->addWidget(m_SceneTree);

	connect(m_DocScene->GetSceneNodeModel(), SIGNAL(IndexesDropped(QModelIndexList)), this, SLOT(SelectIndexRange(QModelIndexList)));
	connect(m_DocScene->GetSceneNodeModel(), SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(OnDataChanged()));
	connect(m_SceneTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(OnSelectionChanged()));

	connect(m_FilterWidget, SIGNAL(FilterChanged(QString)), this, SLOT(OnFilterChanged(QString)));
}

//////////////////////////////////////////////////////////////////////////
SceneNodeNavigator::~SceneNodeNavigator()
{

}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::FitColumns()
{
	m_SceneTree->expandToDepth(0);
	m_SceneTree->resizeColumnToContents(1);	
	m_SceneTree->resizeColumnToContents(0);	

	QTimer::singleShot(0, this, SLOT(ResetSort()));
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::ResetSort()
{
	m_SceneTree->sortByColumn(0, Qt::AscendingOrder);
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::ReApplySort()
{	
	m_SortFilterModel->sort(m_SortFilterModel->sortColumn(), m_SortFilterModel->sortOrder());
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::OnDataChanged()
{
	// doesn't work when called directly from dataChanged()
	QTimer::singleShot(0, this, SLOT(ReApplySort()));
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::SelectIndexRange(const QModelIndexList& indexes)
{
	m_IgnoreSelChanges = true;

	QItemSelectionModel* selModel = m_SceneTree->selectionModel();
	selModel->clearSelection();

	// select all nodes
	qforeach (QModelIndex index, indexes)
	{
		const QModelIndex mainIndex = m_SortFilterModel->mapFromSource(index);
		selModel->select(mainIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
	}

	// focus and scroll to the last one
	if (!indexes.empty())
	{
		const QModelIndex mainIndex = m_SortFilterModel->mapFromSource(indexes.back());

		selModel->setCurrentIndex(mainIndex, QItemSelectionModel::NoUpdate);		
		m_SceneTree->scrollTo(mainIndex, QAbstractItemView::EnsureVisible);
	}

	m_IgnoreSelChanges = false;
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::OnSelectionChanged()
{
	if (m_IgnoreSelChanges) return;

	QList<Ogre::SceneNode*> selNodes;

	qforeach (QModelIndex index, m_SceneTree->selectionModel()->selectedIndexes())
	{
		QModelIndex mainIndex = m_SortFilterModel->mapToSource(index);
		SceneNode* node = m_DocScene->GetSceneNodeModel()->NodeForIndex(mainIndex);
		if (node)
		{
			Ogre::SceneNode* ogreSceneNode = node->GetOgreSceneNode();
			if (ogreSceneNode) selNodes.append(ogreSceneNode);
		}
	}
	m_DocScene->SelectNodes(selNodes);
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeNavigator::OnFilterChanged(const QString& filterStr)
{
	m_SortFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_SortFilterModel->setFilterRegExp(filterStr);
	m_SortFilterModel->sort(m_SortFilterModel->sortColumn(), m_SortFilterModel->sortOrder());
}


} // namespace Armed
