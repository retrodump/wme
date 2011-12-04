// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "WmeWidgetScroll.h"
#include "WmeWidget.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
WmeWidgetScroll::WmeWidgetScroll(QWidget* parent) : QAbstractScrollArea(parent)
{
	m_WmeWidget = new WmeWidget(this);

	setFocusPolicy(Qt::WheelFocus);
	verticalScrollBar()->setSingleStep(20);
	horizontalScrollBar()->setSingleStep(20);
}

//////////////////////////////////////////////////////////////////////////
WmeWidgetScroll::~WmeWidgetScroll()
{

}

//////////////////////////////////////////////////////////////////////////
void WmeWidgetScroll::CreateWmeView()
{
	m_WmeWidget->setObjectName(objectName());
	m_WmeWidget->CreateWmeView();

	SetVirtualSize(1000, 1000);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidgetScroll::resizeEvent(QResizeEvent* event)
{
	m_WmeWidget->move(viewport()->x(), viewport()->y());
	m_WmeWidget->resize(viewport()->width(), viewport()->height());

	UpdateScrollBars();
}

//////////////////////////////////////////////////////////////////////////
void WmeWidgetScroll::SetVirtualSize(const QSize& size)
{
	SetVirtualSize(size.width(), size.height());
}

//////////////////////////////////////////////////////////////////////////
void WmeWidgetScroll::SetVirtualSize(int width, int height)
{
	m_VirtualSize.setWidth(width);
	m_VirtualSize.setHeight(height);

	UpdateScrollBars();
}

//////////////////////////////////////////////////////////////////////////
void WmeWidgetScroll::UpdateScrollBars()
{
	QSize vpSize = viewport()->size();

	verticalScrollBar()->setPageStep(vpSize.height());
	horizontalScrollBar()->setPageStep(vpSize.width());
	verticalScrollBar()->setRange(0, m_VirtualSize.height() - vpSize.height());
	horizontalScrollBar()->setRange(0, m_VirtualSize.width() - vpSize.width());
}

//////////////////////////////////////////////////////////////////////////
bool WmeWidgetScroll::HandleWheelEvent(QWheelEvent* e)
{
	if (e->orientation() == Qt::Horizontal)
	{
		if (horizontalScrollBar()->isVisible())
		{
			QApplication::sendEvent(horizontalScrollBar(), e);
			return true;
		}
	}
	else
	{
		if (verticalScrollBar()->isVisible())
		{
			QApplication::sendEvent(verticalScrollBar(), e);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void WmeWidgetScroll::scrollContentsBy(int dx, int dy)
{
}


} // namespace Armed
