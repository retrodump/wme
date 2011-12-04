// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedWmeWidgetScroll_H__
#define __ArmedWmeWidgetScroll_H__

#include <QWidget>


namespace Armed
{
	class WmeWidget;

	class WmeWidgetScroll : public QAbstractScrollArea
	{
		Q_OBJECT

	public:
		WmeWidgetScroll(QWidget* parent);
		~WmeWidgetScroll();
		void CreateWmeView();

		void SetVirtualSize(const QSize& size);
		void SetVirtualSize(int width, int height);
		bool HandleWheelEvent(QWheelEvent* e);

		void resizeEvent(QResizeEvent* event);		
		
	private:
		void UpdateScrollBars();
		void scrollContentsBy(int dx, int dy);

		WmeWidget* m_WmeWidget;
		QSize m_VirtualSize;
	
	};
}

#endif // __ArmedWmeWidgetScroll_H__
