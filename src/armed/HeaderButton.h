// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef HEADERBUTTON_H
#define HEADERBUTTON_H

#include <QToolButton>


namespace Armed
{
	class HeaderButton : public QToolButton
	{
		Q_OBJECT

	public:
		HeaderButton(QWidget* parent);
		~HeaderButton();

	private:
		void paintEvent (QPaintEvent* event);
	
	};
}

#endif // HEADERBUTTON_H
