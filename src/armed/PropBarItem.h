// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPBARITEM_H
#define PROPBARITEM_H

#include <QWidget>


namespace Armed
{
	class HeaderButton;

	class PropBarItem : public QWidget
	{
		Q_OBJECT

	public:
		PropBarItem(QWidget *parent, QWidget* widget, const QString& label, bool expanded = true);
		~PropBarItem();

		QWidget* GetWidget() const { return m_Widget; }
		QString GetLabel() const { return m_Label; }

		void Collapse();
		void Expand();
		bool IsExpanded() const;

	signals:
		void Expanded();
		void Collapsed();

	private slots:
		void LabelClicked(bool clicked);
		void FocusArea();

	private:
		QWidget* m_Widget;
		QString m_Label;

		HeaderButton* m_HeaderButton;
		QFrame* m_Area;	
	};
}

#endif // PROPBARITEM_H
