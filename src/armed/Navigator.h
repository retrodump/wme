// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedNavigator_H__
#define __ArmedNavigator_H__


#include <QWidget>


namespace Armed
{
	class Navigator : public QWidget
	{
		Q_OBJECT

	public:
		Navigator(QWidget* parent);
		~Navigator();

		void SetNavigator(QWidget* nav);

	private:
		QWidget* m_CurrentNav;
		QWidget* m_CurrentNavParent;
		QVBoxLayout* m_Layout;
	
	};
}

#endif // __ArmedNavigator_H__
