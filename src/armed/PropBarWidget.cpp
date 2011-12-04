// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropBarWidget.h"
#include "PropBarItem.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropBarWidget::PropBarWidget(QWidget* parent) : QScrollArea(parent)
{
	m_Content = new QWidget();

	m_Layout = new QVBoxLayout(m_Content);
	m_Layout->setMargin(0);
	m_Layout->setSpacing(0);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 10000, QSizePolicy::Minimum, QSizePolicy::Expanding);
	m_Layout->addItem(verticalSpacer);
	
	this->setWidgetResizable(true);
	this->setWidget(m_Content);
}

//////////////////////////////////////////////////////////////////////////
PropBarWidget::~PropBarWidget()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
void PropBarWidget::AddItem(const QString& label, QWidget* widget, bool expanded)
{
	PropBarItem* item = new PropBarItem(m_Content, widget, label, true);
	connect(item, SIGNAL(Collapsed()), this, SLOT(OnItemCollapsed()));
	connect(item, SIGNAL(Expanded()), this, SLOT(OnItemExpanded()));

	m_Layout->insertWidget(m_Items.count(), item);

	m_Items.append(item);

	if (!expanded || m_CollapsedLabels.contains(label)) item->Collapse();
}

//////////////////////////////////////////////////////////////////////////
void PropBarWidget::Clear()
{
	qDeleteAll(m_Items);
	m_Items.clear();
}

//////////////////////////////////////////////////////////////////////////
void PropBarWidget::OnItemCollapsed()
{
	PropBarItem* item = qobject_cast<PropBarItem*>(sender());
	if (item) m_CollapsedLabels.insert(item->GetLabel());
}

//////////////////////////////////////////////////////////////////////////
void PropBarWidget::OnItemExpanded()
{
	PropBarItem* item = qobject_cast<PropBarItem*>(sender());
	if (item) m_CollapsedLabels.remove(item->GetLabel());
}


} // namespace Armed
