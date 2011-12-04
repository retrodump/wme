// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropWindow.h"
#include "PropBarWidget.h"
#include "PropSectionBase.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropWindow::PropWindow(QWidget* parent) : QWidget(parent)
{
	m_PropBar = new PropBarWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_PropBar);
}

//////////////////////////////////////////////////////////////////////////
PropWindow::~PropWindow()
{

}

//////////////////////////////////////////////////////////////////////////
QSize PropWindow::sizeHint() const
{
	QSize size = QWidget::sizeHint();
	size.setWidth(std::max(size.width(), 200));
	return size;
}

//////////////////////////////////////////////////////////////////////////
void PropWindow::ClearProperties()
{
	m_Sections.clear();
	m_PropBar->Clear();
}

//////////////////////////////////////////////////////////////////////////
void PropWindow::AddProperty(const QString& sectionName, PropSectionBase* content, bool expanded)
{
	m_PropBar->AddItem(sectionName, content, expanded);
	m_Sections.append(content);
}

//////////////////////////////////////////////////////////////////////////
void PropWindow::DisplayData()
{
	qforeach (PropSectionBase* section, m_Sections)
	{
		section->DisplayData();
	}
}


} // namespace Armed
