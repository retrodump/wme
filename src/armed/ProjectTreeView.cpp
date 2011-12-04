// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ProjectTreeView.h"
#include "ProjectTreeModel.h"
#include "ProjectTreeFilter.h"
#include "Project.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ProjectTreeView::ProjectTreeView(QWidget* parent) : QTreeView(parent)
{
	setHeaderHidden(true);

	m_Model = new ProjectTreeModel(this);
	m_Model->setReadOnly(true);

	m_FilterModel = new ProjectTreeFilter(this);
	m_FilterModel->setSourceModel(m_Model);

	setModel(m_FilterModel);

	connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(OnSelectionChanged()));

	SetProjectRoot();
}

//////////////////////////////////////////////////////////////////////////
ProjectTreeView::~ProjectTreeView()
{

}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::SetProjectRoot()
{
	QString projectRoot = Project::GetInstance()->GetProjectRoot();

	m_Model->setRootPath(projectRoot);

	QModelIndex idx = m_Model->index(projectRoot);
	const QModelIndex mainIndex = m_FilterModel->mapFromSource(idx);
	setRootIndex(mainIndex);
}

//////////////////////////////////////////////////////////////////////////
QString ProjectTreeView::GetCurrentFile() const
{
	if (selectedIndexes().count() > 0)
	{
		QModelIndex idx = m_FilterModel->mapToSource(selectedIndexes().at(0));
		if (idx.isValid()) return Project::GetInstance()->GetRelativeFileName(m_Model->filePath(idx));
	}
	return QString();
}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::SetCurrentFile(const QString& fileName)
{
	QString fullFileName = Project::GetInstance()->GetAbsoluteFileName(fileName);

	QModelIndex idx = m_Model->index(fullFileName);
	if (idx.isValid())
	{
		PopulatePath(fileName);

		const QModelIndex mainIndex = m_FilterModel->mapFromSource(idx);
		setCurrentIndex(mainIndex);

		if (QFileInfo(fullFileName).isDir()) expand(mainIndex);
		
		// ok, this is ugly, but doesn't seem to work otherwise
		// is there a better way?
		QTimer::singleShot(500, this, SLOT(ScrollToCurrent()));
		//scrollTo(currentIndex(), QAbstractItemView::EnsureVisible);
	}
}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::ScrollToCurrent()
{
	scrollTo(currentIndex(), QAbstractItemView::EnsureVisible);
}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::SetFilters(const QStringList& filters)
{
	m_Model->setNameFilterDisables(false);
	m_Model->setNameFilters(filters);
}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::SetUserFilter(const QString& filter)
{
	QString modFilter = filter;
	if (!modFilter.contains('*') && !modFilter.contains('?'))
		modFilter = "*" + modFilter + "*";

	QStringList filters;
	filters << modFilter;

	m_Model->setNameFilterDisables(false);
	m_Model->setNameFilters(filters);
}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::OnSelectionChanged()
{
	emit PathSelected(GetCurrentFile());
}

//////////////////////////////////////////////////////////////////////////
void ProjectTreeView::PopulatePath(const QString& path)
{
	QString projectPath = QtUtil::NormalizeFileName(Project::GetInstance()->GetProjectRoot());

	QString absPath = Project::GetInstance()->GetAbsoluteFileName(path);
	absPath = QtUtil::NormalizeFileName(absPath);

	QStringList parts = absPath.split(QDir::fromNativeSeparators(QDir::separator()));
	absPath.clear();
	qforeach(QString part, parts)
	{
		if (!absPath.isEmpty()) absPath += QDir::separator();
		absPath = QtUtil::NormalizeFileName(absPath + part);

		if (!absPath.startsWith(projectPath)) continue;

		if (QFileInfo(absPath).isDir())
		{
			QModelIndex idx = m_Model->index(absPath);
			if (idx.isValid())
			{
				while (m_Model->canFetchMore(idx))
				{
					m_Model->fetchMore(idx);
				}
			}
		}
	}
}


} // namespace Armed
