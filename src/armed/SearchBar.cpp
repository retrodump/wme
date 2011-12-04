// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SearchBar.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
SearchBar::SearchBar(QWidget* parent) : QWidget(parent)
{
	m_SettingText = false;

	installEventFilter(this);

	m_Ui.setupUi(this);

	m_Ui.InfoLabel->hide();

	connect(m_Ui.CloseButton, SIGNAL(clicked()), SLOT(hide()));
	connect(m_Ui.TextField, SIGNAL(textChanged(QString)), this, SLOT(TextChanged(QString)));
	connect(m_Ui.TextField, SIGNAL(returnPressed()), this, SIGNAL(FindNext()));
	connect(m_Ui.TextField, SIGNAL(textChanged(QString)), this, SLOT(SetState()));
	connect(m_Ui.FindPrevButton, SIGNAL(clicked()), this, SIGNAL(FindPrev()));
	connect(m_Ui.FindNextButton, SIGNAL(clicked()), this, SIGNAL(FindNext()));

	SetState();
}

//////////////////////////////////////////////////////////////////////////
SearchBar::~SearchBar()
{

}

//////////////////////////////////////////////////////////////////////////
void SearchBar::Show()
{
	show();
	m_Ui.TextField->selectAll();
	m_Ui.TextField->setFocus(Qt::ShortcutFocusReason);
}

//////////////////////////////////////////////////////////////////////////
void SearchBar::ShowEmpty()
{
	Show();
	m_Ui.TextField->clear();
}

//////////////////////////////////////////////////////////////////////////
QString SearchBar::GetText() const
{
	return m_Ui.TextField->text();
}

//////////////////////////////////////////////////////////////////////////
void SearchBar::SetText(const QString& text)
{
	m_SettingText = true;

	m_Ui.TextField->setText(text);
	m_Ui.TextField->selectAll();
	SetColors(true);

	m_SettingText = false;
}

//////////////////////////////////////////////////////////////////////////
bool SearchBar::GetCaseSensitive() const
{
	return m_Ui.CaseCheckbox->isChecked();
}

//////////////////////////////////////////////////////////////////////////
void SearchBar::SetColors(bool found)
{
	QPalette palette = m_Ui.TextField->palette();
	palette.setColor(QPalette::Active, QPalette::Base, found ? Qt::white : QColor(255, 102, 102));
	palette.setColor(QPalette::Active, QPalette::Text, found ? Qt::black : Qt::white);
	m_Ui.TextField->setPalette(palette);
}

//////////////////////////////////////////////////////////////////////////
void SearchBar::SetState()
{
	bool hasText = !m_Ui.TextField->text().isEmpty();

	m_Ui.FindNextButton->setEnabled(hasText);
	m_Ui.FindPrevButton->setEnabled(hasText);
}

//////////////////////////////////////////////////////////////////////////
void SearchBar::TextChanged(const QString& text)
{
	if (!m_SettingText)	emit Find(text, true);
}

//////////////////////////////////////////////////////////////////////////
bool SearchBar::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		if ((static_cast<QKeyEvent*>(event))->key() == Qt::Key_Escape)
		{
			hide();
			emit EscapePressed();
		}
	}
	return QWidget::eventFilter(object, event);
}



} // namespace Armed
