// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ProjectBrowser.h"
#include "ProjectTreeView.h"
#include "Project.h"
#include "FilterWidget.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ProjectBrowser::ProjectBrowser(QWidget* parent) : QWidget(parent)
{
	m_FilterWidget = new FilterWidget(this);

	m_ProjectTree = new ProjectTreeView(this);
	m_ProjectTree->setHeaderHidden(true);

	m_ProjectTree->setDragDropMode(QAbstractItemView::DragOnly);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_FilterWidget);
	layout->addWidget(m_ProjectTree);

	connect(Project::GetInstance(), SIGNAL(ProjectChanged(QString)), this, SLOT(OnProjectChanged()));

	connect(m_FilterWidget, SIGNAL(FilterChanged(QString)), this, SLOT(OnFilterChanged(QString)));
}

//////////////////////////////////////////////////////////////////////////
ProjectBrowser::~ProjectBrowser()
{

}

//////////////////////////////////////////////////////////////////////////
void ProjectBrowser::OnProjectChanged()
{
	m_ProjectTree->SetProjectRoot();
}

//////////////////////////////////////////////////////////////////////////
void ProjectBrowser::OnFilterChanged(const QString& filterStr)
{
	m_ProjectTree->SetUserFilter(filterStr);
}



} // namespace Armed
