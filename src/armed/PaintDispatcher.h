// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPaintDispatcher_H__
#define __ArmedPaintDispatcher_H__


#include <QWidget>


namespace Armed
{
	class PaintDispatcher : public QWidget
	{
		Q_OBJECT

	public:
		PaintDispatcher(QWidget* parent);
		~PaintDispatcher();

		void paintEvent(QPaintEvent* event);

	private:	
	};
}

#endif // __ArmedPaintDispatcher_H__
