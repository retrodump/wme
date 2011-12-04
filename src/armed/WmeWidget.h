// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedWmeWidget_H__
#define __ArmedWmeWidget_H__

#include <QWidget>
#include "EmbeddedWindowBridge.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"

using namespace Wme;

namespace Wme
{
	class EmbeddedWindow;
}

namespace Armed
{
	class WmeWidget : public QWidget, public EmbeddedWindowBridge
	{
		Q_OBJECT

	public:
		WmeWidget(QWidget* parent);
		~WmeWidget();

		void CreateWmeView();

		EmbeddedWindow* GetWmeView() const { return m_WmeWindow; }

		void AddIgnoredKeySequence(QKeySequence::StandardKey keySequence);

		// QWidget
		void paintEvent(QPaintEvent* event);
		QPaintEngine* paintEngine() const;
		void resizeEvent(QResizeEvent* event);

		void wheelEvent(QWheelEvent* event);
		void mouseDoubleClickEvent(QMouseEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

		void keyPressEvent(QKeyEvent* event);
		void keyReleaseEvent(QKeyEvent* event);

		void showEvent(QShowEvent* event);
		void hideEvent(QHideEvent* event);

		// EmbeddedViewBridge
		virtual Wme::AnsiString GetWindowId() const;
		virtual void GetWindowSize(size_t& width, size_t& height);

		virtual void GetMousePos(int& x, int& y) const;
		virtual void SetMousePos(int x, int y);
		virtual bool IsShiftDown() const;
		virtual bool IsControlDown() const;
		virtual bool IsAltDown() const;

	private:		
		Wme::EmbeddedWindow* m_WmeWindow;
		QList<QKeySequence::StandardKey> m_IgnoredKeySequences;

		static MouseEvent::MouseButton ConvertQtButton(Qt::MouseButton qtButton);
		static KeyboardEvent::KeyCode ConvertQtKey(QKeyEvent* event);
	};
}

#endif // __ArmedWmeWidget_H__
