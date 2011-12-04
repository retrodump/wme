// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropBarItem.h"
#include "PropBarWidget.h"
#include "HeaderButton.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropBarItem::PropBarItem(QWidget* parent, QWidget* widget, const QString& label, bool expanded) : QWidget(parent)
{
	m_Widget = widget;
	m_Label = label;

	m_HeaderButton = new HeaderButton(this);
	m_HeaderButton->setText(label);
	m_HeaderButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_HeaderButton->setCheckable(true);
	m_HeaderButton->setChecked(expanded);


	m_Area = new QFrame(this);
	m_Area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_Area->setVisible(expanded);

	QVBoxLayout* areaLayout = new QVBoxLayout(m_Area);
	areaLayout->setMargin(0);
	areaLayout->setSpacing(0);
	areaLayout->addWidget(widget);


	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_HeaderButton);
	mainLayout->addWidget(m_Area);

	this->setLayout(mainLayout);

	connect(m_HeaderButton, SIGNAL(clicked(bool)), this, SLOT(LabelClicked(bool)));

	QWidget::setTabOrder(m_HeaderButton, widget);
}

//////////////////////////////////////////////////////////////////////////
PropBarItem::~PropBarItem()
{

}

//////////////////////////////////////////////////////////////////////////
void PropBarItem::Collapse()
{
	if (IsExpanded())
	{
		m_HeaderButton->setChecked(false);
		LabelClicked(false);
	}
}

//////////////////////////////////////////////////////////////////////////
void PropBarItem::Expand()
{
	if (!IsExpanded())
	{
		m_HeaderButton->setChecked(true);
		LabelClicked(true);
	}
}

//////////////////////////////////////////////////////////////////////////
bool PropBarItem::IsExpanded() const
{
	return m_HeaderButton->isChecked();
}

//////////////////////////////////////////////////////////////////////////
void PropBarItem::LabelClicked(bool clicked)
{
	m_Area->setVisible(clicked);
	if (clicked) QTimer::singleShot(0, this, SLOT(FocusArea()));

	if (clicked) emit Expanded();
	else emit Collapsed();
}

//////////////////////////////////////////////////////////////////////////
void PropBarItem::FocusArea()
{
	QWidget* w = this->parentWidget();
	while (w)
	{
		QScrollArea* p = qobject_cast<PropBarWidget*>(w);
		if (p)
		{
			p->ensureWidgetVisible(this);
			break;
		}
		w = w->parentWidget();
	}

}


} // namespace Armed
