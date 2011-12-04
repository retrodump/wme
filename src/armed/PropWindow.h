// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPropWindow_H__
#define __ArmedPropWindow_H__


#include <QWidget>


namespace Armed
{
	class PropBarWidget;
	class PropSectionBase;

	class PropWindow : public QWidget
	{
		Q_OBJECT

	public:
		PropWindow(QWidget* parent);
		~PropWindow();
		QSize sizeHint() const;

		void ClearProperties();
		void AddProperty(const QString& sectionName, PropSectionBase* content, bool expanded = true);

		void DisplayData();

	private:
		PropBarWidget* m_PropBar;
		QVector<PropSectionBase*> m_Sections;	
	};
}

#endif // __ArmedPropWindow_H__
