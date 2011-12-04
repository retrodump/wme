// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPBARWIDGET_H
#define PROPBARWIDGET_H

#include <QScrollArea>


namespace Armed
{
	class PropBarItem;

	class PropBarWidget : public QScrollArea
	{
		Q_OBJECT

	public:
		PropBarWidget(QWidget *parent);
		~PropBarWidget();

		void AddItem(const QString& label, QWidget* widget, bool expanded = true);
		void Clear();

	private slots:
		void OnItemCollapsed();
		void OnItemExpanded();

	private:
		QVector<PropBarItem*> m_Items;
		QVBoxLayout* m_Layout;
		QWidget* m_Content;

		QSet<QString> m_CollapsedLabels;
	};
}

#endif // PROPBARWIDGET_H
