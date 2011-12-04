// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "HeaderButton.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
HeaderButton::HeaderButton(QWidget *parent) : QToolButton(parent)
{
	QFont font = this->font();
	font.setBold(true);
	setFont(font);
}

//////////////////////////////////////////////////////////////////////////
HeaderButton::~HeaderButton()
{
}


//////////////////////////////////////////////////////////////////////////
void HeaderButton::paintEvent (QPaintEvent* event)
{
	QStylePainter painter(this);

	QStyleOptionToolButton buttonOption;
	initStyleOption(&buttonOption);
	buttonOption.state &= ~QStyle::State_On;
	buttonOption.state |= QStyle::State_Raised;

	painter.drawComplexControl(QStyle::CC_ToolButton, buttonOption);


	QStyleOption branchOption;
	int i = 10; // indent
	QRect r = rect();
	branchOption.rect = QRect(r.left() + i / 2, r.top() + (r.height() - i) / 2, i, i);
	branchOption.palette = this->palette();
	branchOption.state = QStyle::State_Children;
	if (this->isChecked()) branchOption.state |= QStyle::State_Open;
	//if (buttonOption.state & QStyle::State_MouseOver) branchOption.state |= QStyle::State_MouseOver;

	style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &painter);
}


} // namespace Armed

