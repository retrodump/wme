// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedSearchBar_H__
#define __ArmedSearchBar_H__


#include <QWidget>
#include "ui_SearchBar.h"


namespace Armed
{
	class SearchBar : public QWidget
	{
		Q_OBJECT

	public:
		SearchBar(QWidget* parent = 0);
		~SearchBar();

		void Show();
		void ShowEmpty();

		QString GetText() const;
		void SetText(const QString& text);
		bool GetCaseSensitive() const;

		void SetColors(bool found);

	signals:
		void EscapePressed();

		void FindNext();
		void FindPrev();
		void Find(const QString& text, bool forward);

	private slots:
		void SetState();
		void TextChanged(const QString& text);

	private:
		bool eventFilter(QObject* object, QEvent* event);
		Ui::SearchBarClass m_Ui;
		bool m_SettingText;
	};
}

#endif // __ArmedSearchBar_H__
