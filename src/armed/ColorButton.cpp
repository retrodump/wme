// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ColorButton.h"
#include "QtUtil.h"


namespace Armed
{


ColorButton* ColorButton::s_CurrentButton = NULL;
QColorDialog* ColorButton::s_Dialog = NULL;

//////////////////////////////////////////////////////////////////////////
ColorButton::ColorButton(QWidget* parent, const QColor& color) : QPushButton(parent)
{
	m_Color = color;
	m_AlphaChannel = true;
	m_ModalDialog = false;
	setCheckable(true);
	SetTooltip();

	connect(this, SIGNAL(toggled(bool)), this, SLOT(OnToggled(bool)));
}

//////////////////////////////////////////////////////////////////////////
ColorButton::~ColorButton()
{
	if (s_CurrentButton == this) CloseDialog();
}


//////////////////////////////////////////////////////////////////////////
void ColorButton::SetColor(const QColor& color)
{
	if (color != m_Color)
	{
		m_Color = color;
		SetTooltip();
		update();
		emit ColorChanged(color);
	}
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::SetAlphaChannel(bool alphaChannel)
{
	m_AlphaChannel = alphaChannel;
	SetTooltip();
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::SetTooltip()
{
	QString tooltip;
	if (m_AlphaChannel) tooltip.append(QString(tr("R: %1<br/>G: %2<br/>B: %3<br/>A: %4")).arg(m_Color.red()).arg(m_Color.green()).arg(m_Color.blue()).arg(m_Color.alpha()));
	else tooltip.append(QString(tr("R: %1<br/>G: %2<br/>B: %3")).arg(m_Color.red()).arg(m_Color.green()).arg(m_Color.blue()));

	setToolTip(tooltip);
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::paintEvent(QPaintEvent* event)
{
	QPushButton::paintEvent(event);

	QStylePainter painter(this);

	int arrowSize = 10;
	int border = 4;

	QStyleOption arrowOpt;
	arrowOpt.initFrom(this);
	QPalette pal = arrowOpt.palette;
	pal.setBrush(QPalette::All, QPalette::Text, QColor(0, 0, 0));
	arrowOpt.rect = QRect(width() - arrowSize - border, height() / 2 - arrowSize / 2, arrowSize, arrowSize);
	arrowOpt.palette = pal;
	style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &painter, this);

	
	QRect colorRect(border, border, width() - (3 * border + arrowSize) - 1, height() - 2 * border - 1);

	if (m_Color.alpha() < 255)
	{
		painter.drawTiledPixmap(colorRect.adjusted(1, 1, -1, -1), QtUtil::TilePixMap(5));
	}

	painter.setPen(Qt::black);
	painter.setBrush(m_Color);
	painter.drawRect(colorRect);
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::OnToggled(bool checked)
{
	if (!checked)
	{
		if (s_CurrentButton == this) CloseDialog();
	}
	else
	{
		if (s_CurrentButton) s_CurrentButton->Commit();
		s_CurrentButton = this;
		ShowDialog();
	}
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::OnDialogColorChanged(const QColor& color)
{
	if (!m_ModalDialog) SetColor(color);	
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::OnDialogCommitted()
{
	if (m_ModalDialog)
	{
		if (s_Dialog) SetColor(s_Dialog->currentColor());
	}
	setChecked(false);
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::OnDialogCancelled()
{
	if (!m_ModalDialog) SetColor(m_OrigColor);

	setChecked(false);
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::Commit()
{
	setChecked(false);
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::CloseDialog()
{
	s_CurrentButton = NULL;
	if (s_Dialog)
	{
		if (!m_ModalDialog)
		{
			QSettings settings;
			settings.beginGroup("ColorDialog");
			settings.setValue("Geometry", s_Dialog->saveGeometry());
			settings.endGroup();

			s_Dialog->accept();
		}

		s_Dialog->deleteLater();
	}
}

//////////////////////////////////////////////////////////////////////////
void ColorButton::ShowDialog()
{
	s_CurrentButton = this;
	m_OrigColor = m_Color;

	s_Dialog = new QColorDialog(m_Color, this);

	if (!m_ModalDialog)
	{
		QSettings settings;
		settings.beginGroup("ColorDialog");
		s_Dialog->restoreGeometry(settings.value("Geometry").toByteArray());
		settings.endGroup();
	}

	if (m_AlphaChannel) s_Dialog->setOption(QColorDialog::ShowAlphaChannel);

	s_Dialog->connect(s_Dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(OnDialogColorChanged(QColor)));
	s_Dialog->connect(s_Dialog, SIGNAL(accepted()), this, SLOT(OnDialogCommitted()));
	s_Dialog->connect(s_Dialog, SIGNAL(rejected()), this, SLOT(OnDialogCancelled()));

	s_Dialog->setModal(m_ModalDialog);
	s_Dialog->show();
}


} // namespace Armed