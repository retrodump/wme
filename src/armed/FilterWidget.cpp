// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "FilterWidget.h"


namespace Armed
{

//////////////////////////////////////////////////////////////////////////
IconButton::IconButton(QWidget* parent) : QToolButton(parent)
{
	setCursor(Qt::ArrowCursor);
	setFocusPolicy(Qt::NoFocus);
}

//////////////////////////////////////////////////////////////////////////
void IconButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	QIcon::Mode state = QIcon::Disabled;
	if (isEnabled())
		state = isDown() ? QIcon::Selected : QIcon::Normal;
	QPixmap iconpixmap = icon().pixmap(QSize(s_ButtonSize, s_ButtonSize), state, QIcon::Off);
	QRect pixmapRect = QRect(0, 0, iconpixmap.width(), iconpixmap.height());
	pixmapRect.moveCenter(rect().center());
	painter.setOpacity(0.7);
	painter.drawPixmap(pixmapRect, iconpixmap);
}

//////////////////////////////////////////////////////////////////////////
FilterWidget::FilterWidget(QWidget* parent)	: QWidget(parent)
{
	m_Editor = new QLineEdit(this);
	m_ClearButton = new IconButton(m_Editor);

	m_Editor->setPlaceholderText(tr("Filter"));


	QSize size(IconButton::s_ButtonSize + 6, IconButton::s_ButtonSize + 2);

	QMargins margins = m_Editor->textMargins();
	margins.setRight(size.width());
	m_Editor->setTextMargins(margins);


	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(m_Editor);


	QIcon icon(":/icons/cleartext.png");
	m_ClearButton->setIcon(icon);
	m_ClearButton->setToolTip(tr("Clear text"));
	m_ClearButton->setVisible(false);

	connect(m_ClearButton, SIGNAL(clicked()), this, SLOT(Reset()));
	connect(m_Editor, SIGNAL(textChanged(QString)), this, SLOT(CheckButton(QString)));
	connect(m_Editor, SIGNAL(textEdited(QString)), this, SIGNAL(FilterChanged(QString)));
}

//////////////////////////////////////////////////////////////////////////
FilterWidget::~FilterWidget()
{

}

//////////////////////////////////////////////////////////////////////////
QString FilterWidget::text() const
{
	return m_Editor->text();
}

//////////////////////////////////////////////////////////////////////////
void FilterWidget::CheckButton(const QString& text)
{
	if (m_OldText.isEmpty() || text.isEmpty())
		m_ClearButton->setVisible(!m_Editor->text().isEmpty());
	m_OldText = text;
}

//////////////////////////////////////////////////////////////////////////
void FilterWidget::Reset()
{
	if (!m_Editor->text().isEmpty())
	{
		m_Editor->clear();
		emit FilterChanged(QString());
	}
}

//////////////////////////////////////////////////////////////////////////
void FilterWidget::resizeEvent(QResizeEvent*)
{
	QRect contentRect = m_Editor->rect();

	const int iconOffset = m_Editor->textMargins().right() + 0;
	m_ClearButton->setGeometry(contentRect.adjusted(m_Editor->width() - iconOffset, 0, 0, 0));
}

//////////////////////////////////////////////////////////////////////////
void FilterWidget::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Escape) Reset();
}


} // namespace Armed
