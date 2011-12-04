// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "WmeWidget.h"
#include "WmeWidgetScroll.h"
#include "Game.h"
#include "ContentManager.h"
#include "EmbeddedWindow.h"
#include "QtUtil.h"

using namespace Wme;

namespace Armed
{


//////////////////////////////////////////////////////////////////////////
WmeWidget::WmeWidget(QWidget* parent) : QWidget(parent)
{
	setFocusPolicy(Qt::WheelFocus);
	setAttribute(Qt::WA_NoBackground);
	setAttribute(Qt::WA_PaintOnScreen);

	m_WmeWindow = NULL;
}

//////////////////////////////////////////////////////////////////////////
WmeWidget::~WmeWidget()
{
	if (m_WmeWindow)
	{
		Game::GetInstance()->GetContentMgr()->RemoveView(m_WmeWindow);
		m_WmeWindow = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::CreateWmeView()
{
	m_WmeWindow = Game::GetInstance()->GetContentMgr()->AddEmbeddedWindow(QtUtil::QStringToWide(this->objectName()), this);
	m_WmeWindow->OnResize();
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::AddIgnoredKeySequence(QKeySequence::StandardKey keySequence)
{
	if (!m_IgnoredKeySequences.contains(keySequence)) m_IgnoredKeySequences.append(keySequence);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::paintEvent(QPaintEvent* event)
{
	if (!isVisible()) return;
	// ...
}

//////////////////////////////////////////////////////////////////////////
QPaintEngine* WmeWidget::paintEngine() const
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::resizeEvent(QResizeEvent* event)
{
	if (m_WmeWindow) m_WmeWindow->OnResize();
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::wheelEvent(QWheelEvent* event)
{
	if (!m_WmeWindow) return;

	WmeWidgetScroll* parentWidg = qobject_cast<WmeWidgetScroll*>(parent());
	if (parentWidg)
	{
		if (parentWidg->HandleWheelEvent(event)) return;
	}

	if (event->orientation() == Qt::Vertical)
	{
		MouseEvent::EventType eventType = event->delta() > 0 ? MouseEvent::EVENT_WHEEL_UP : MouseEvent::EVENT_WHEEL_DOWN;

		MouseEvent e(eventType, MouseEvent::BUTTON_NONE, event->x(), event->y());
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(m_WmeWindow->GetOgreWindow(), e);
	}
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!m_WmeWindow) return;
	MouseEvent e(MouseEvent::EVENT_DOUBLE_CLICK, ConvertQtButton(event->button()), event->x(), event->y());
	Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(m_WmeWindow->GetOgreWindow(), e);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::mousePressEvent(QMouseEvent* event)
{
	if (!m_WmeWindow) return;
	MouseEvent e(MouseEvent::EVENT_PRESS, ConvertQtButton(event->button()), event->x(), event->y());
	Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(m_WmeWindow->GetOgreWindow(), e);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (!m_WmeWindow) return;
	MouseEvent e(MouseEvent::EVENT_RELEASE, ConvertQtButton(event->button()), event->x(), event->y());
	Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(m_WmeWindow->GetOgreWindow(), e);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::keyPressEvent(QKeyEvent* event)
{
	// ignored sequences
	qforeach (QKeySequence::StandardKey sequence, m_IgnoredKeySequences)
	{
		if (event->matches(sequence))
		{
			event->ignore();
			return;
		}
	}

	if (!m_WmeWindow) return;
	KeyboardEvent e(KeyboardEvent::EVENT_PRESS, ConvertQtKey(event), QtUtil::QStringToWide(event->text()));
	Game::GetInstance()->GetContentMgr()->InvokeKeyboardEvent(m_WmeWindow, e);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (!m_WmeWindow) return;
	KeyboardEvent e(KeyboardEvent::EVENT_RELEASE, ConvertQtKey(event), QtUtil::QStringToWide(event->text()));
	Game::GetInstance()->GetContentMgr()->InvokeKeyboardEvent(m_WmeWindow, e);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::showEvent(QShowEvent* event)
{
	if (m_WmeWindow) m_WmeWindow->GetOgreWindow()->setAutoUpdated(true);
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::hideEvent(QHideEvent* event)
{
	// disable rendering for hidden widgets
	if (m_WmeWindow) m_WmeWindow->GetOgreWindow()->setAutoUpdated(false);
}

//////////////////////////////////////////////////////////////////////////
AnsiString WmeWidget::GetWindowId() const
{
#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
	return QString::number((unsigned long)this->winId()).toStdString();
#else
	const QX11Info info = this->x11Info();

	AnsiString winHandle;
	winHandle  = QString::number((unsigned long)(info.display())).toStdString();
	winHandle += ":";
	winHandle += QString::number((unsigned int)(info.screen())).toStdString();
	winHandle += ":";
	winHandle += QString::number((unsigned long)(this->winId())).toStdString();
	winHandle += ":";
	winHandle += QString::number((unsigned long)(info.visual())).toStdString();

	return winHandle;
#endif
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::GetWindowSize(size_t& width, size_t& height)
{
	width = this->width();
	height = this->height();
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::GetMousePos(int& x, int& y) const
{
	QPoint curPos = mapFromGlobal(QCursor::pos());
	x = curPos.x();
	y = curPos.y();
}

//////////////////////////////////////////////////////////////////////////
void WmeWidget::SetMousePos(int x, int y)
{
	QPoint curPos(x, y);
	QCursor::setPos(mapToGlobal(curPos));
}

//////////////////////////////////////////////////////////////////////////
bool WmeWidget::IsShiftDown() const
{
	return QApplication::keyboardModifiers() & Qt::ShiftModifier;
}

//////////////////////////////////////////////////////////////////////////
bool WmeWidget::IsControlDown() const
{
	return QApplication::keyboardModifiers() & Qt::ControlModifier;
}

//////////////////////////////////////////////////////////////////////////
bool WmeWidget::IsAltDown() const
{
	return QApplication::keyboardModifiers() & Qt::AltModifier;
}

//////////////////////////////////////////////////////////////////////////
MouseEvent::MouseButton WmeWidget::ConvertQtButton(Qt::MouseButton qtButton)
{
	switch (qtButton)
	{
	case Qt::LeftButton:
		return MouseEvent::BUTTON_LEFT;
	case Qt::RightButton:
		return MouseEvent::BUTTON_RIGHT;
	case Qt::MidButton:
		return MouseEvent::BUTTON_MIDDLE;
	default:
		return MouseEvent::BUTTON_NONE;
	}
}

//////////////////////////////////////////////////////////////////////////
KeyboardEvent::KeyCode WmeWidget::ConvertQtKey(QKeyEvent* event)
{
	if (event->modifiers() & Qt::KeypadModifier)
	{
		switch (event->key())
		{
		case Qt::Key_7:
			return KeyboardEvent::KEY_NUMPAD7;
		case Qt::Key_8:
			return KeyboardEvent::KEY_NUMPAD8;
		case Qt::Key_9:
			return KeyboardEvent::KEY_NUMPAD9;
		case Qt::Key_Minus:
			return KeyboardEvent::KEY_SUBTRACT;
		case Qt::Key_4:
			return KeyboardEvent::KEY_NUMPAD4;
		case Qt::Key_5:
			return KeyboardEvent::KEY_NUMPAD5;
		case Qt::Key_6:
			return KeyboardEvent::KEY_NUMPAD6;
		case Qt::Key_Plus:
			return KeyboardEvent::KEY_ADD;
		case Qt::Key_1:
			return KeyboardEvent::KEY_NUMPAD1;
		case Qt::Key_2:
			return KeyboardEvent::KEY_NUMPAD2;
		case Qt::Key_3:
			return KeyboardEvent::KEY_NUMPAD3;
		case Qt::Key_0:
			return KeyboardEvent::KEY_NUMPAD0;
		case Qt::Key_Period:
			return KeyboardEvent::KEY_DECIMAL;
		case Qt::Key_Equal:
			return KeyboardEvent::KEY_NUMPADEQUALS;
		case Qt::Key_Return:
			return KeyboardEvent::KEY_NUMPADENTER;
		case Qt::Key_Comma:
			return KeyboardEvent::KEY_NUMPADCOMMA;
		default:
			return KeyboardEvent::KEY_UNASSIGNED;
		}
	}
	else
	{
		switch (event->key())
		{
		case Qt::Key_Escape:	
			return KeyboardEvent::KEY_ESCAPE;
		case Qt::Key_1:
			return KeyboardEvent::KEY_1;
		case Qt::Key_2:
			return KeyboardEvent::KEY_2;
		case Qt::Key_3:
			return KeyboardEvent::KEY_3;
		case Qt::Key_4:
			return KeyboardEvent::KEY_4;
		case Qt::Key_5:
			return KeyboardEvent::KEY_5;
		case Qt::Key_6:
			return KeyboardEvent::KEY_6;
		case Qt::Key_7:
			return KeyboardEvent::KEY_7;
		case Qt::Key_8:	
			return KeyboardEvent::KEY_8;
		case Qt::Key_9:
			return KeyboardEvent::KEY_9;
		case Qt::Key_0:
			return KeyboardEvent::KEY_0;
		case Qt::Key_Minus:
			return KeyboardEvent::KEY_MINUS;
		case Qt::Key_Equal:
			return KeyboardEvent::KEY_EQUALS;
		case Qt::Key_Backspace:
			return KeyboardEvent::KEY_BACK;
		case Qt::Key_Tab:
			return KeyboardEvent::KEY_TAB;
		case Qt::Key_Q:
			return KeyboardEvent::KEY_Q;
		case Qt::Key_W:
			return KeyboardEvent::KEY_W;
		case Qt::Key_E:
			return KeyboardEvent::KEY_E;
		case Qt::Key_R:
			return KeyboardEvent::KEY_R;
		case Qt::Key_T:
			return KeyboardEvent::KEY_T;
		case Qt::Key_Y:
			return KeyboardEvent::KEY_Y;
		case Qt::Key_U:
			return KeyboardEvent::KEY_U;
		case Qt::Key_I:
			return KeyboardEvent::KEY_I;
		case Qt::Key_O:
			return KeyboardEvent::KEY_O;
		case Qt::Key_P:
			return KeyboardEvent::KEY_P;
		case Qt::Key_BracketLeft:
			return KeyboardEvent::KEY_LBRACKET;
		case Qt::Key_BracketRight:
			return KeyboardEvent::KEY_RBRACKET;
		case Qt::Key_Return:
			return KeyboardEvent::KEY_RETURN;
		case Qt::Key_Control:
			return KeyboardEvent::KEY_LCONTROL;
		case Qt::Key_A:
			return KeyboardEvent::KEY_A;
		case Qt::Key_S:
			return KeyboardEvent::KEY_S;
		case Qt::Key_D:
			return KeyboardEvent::KEY_D;
		case Qt::Key_F:
			return KeyboardEvent::KEY_F;
		case Qt::Key_G:
			return KeyboardEvent::KEY_G;
		case Qt::Key_H:
			return KeyboardEvent::KEY_H;
		case Qt::Key_J:
			return KeyboardEvent::KEY_J;
		case Qt::Key_K:
			return KeyboardEvent::KEY_K;
		case Qt::Key_L:
			return KeyboardEvent::KEY_L;
		case Qt::Key_Semicolon:
			return KeyboardEvent::KEY_SEMICOLON;
		case Qt::Key_Apostrophe:
			return KeyboardEvent::KEY_APOSTROPHE;
		case Qt::Key_Dead_Grave:
			return KeyboardEvent::KEY_GRAVE;
		case Qt::Key_Shift:
			return KeyboardEvent::KEY_LSHIFT;
		case Qt::Key_Backslash:
			return KeyboardEvent::KEY_BACKSLASH;
		case Qt::Key_Z:
			return KeyboardEvent::KEY_Z;
		case Qt::Key_X:
			return KeyboardEvent::KEY_X;
		case Qt::Key_C:
			return KeyboardEvent::KEY_C;
		case Qt::Key_V:
			return KeyboardEvent::KEY_V;
		case Qt::Key_B:
			return KeyboardEvent::KEY_B;
		case Qt::Key_N:
			return KeyboardEvent::KEY_N;
		case Qt::Key_M:
			return KeyboardEvent::KEY_M;
		case Qt::Key_Comma:
			return KeyboardEvent::KEY_COMMA;
		case Qt::Key_Period:
			return KeyboardEvent::KEY_PERIOD;
		case Qt::Key_Slash:
			return KeyboardEvent::KEY_SLASH;
		case Qt::Key_multiply:
			return KeyboardEvent::KEY_MULTIPLY;
		case Qt::Key_Menu:
			return KeyboardEvent::KEY_LMENU;
		case Qt::Key_Space:
			return KeyboardEvent::KEY_SPACE;
		case Qt::Key_F1:
			return KeyboardEvent::KEY_F1;
		case Qt::Key_F2:
			return KeyboardEvent::KEY_F2;
		case Qt::Key_F3:
			return KeyboardEvent::KEY_F3;
		case Qt::Key_F4:
			return KeyboardEvent::KEY_F4;
		case Qt::Key_F5:
			return KeyboardEvent::KEY_F5;
		case Qt::Key_F6:
			return KeyboardEvent::KEY_F6;
		case Qt::Key_F7:
			return KeyboardEvent::KEY_F7;
		case Qt::Key_F8:
			return KeyboardEvent::KEY_F8;
		case Qt::Key_F9:
			return KeyboardEvent::KEY_F9;
		case Qt::Key_F10:
			return KeyboardEvent::KEY_F10;
		case Qt::Key_NumLock:
			return KeyboardEvent::KEY_NUMLOCK;
		case Qt::Key_ScrollLock:
			return KeyboardEvent::KEY_SCROLL;
		case Qt::Key_F11:
			return KeyboardEvent::KEY_F11;
		case Qt::Key_F12:
			return KeyboardEvent::KEY_F12;
		case Qt::Key_F13:
			return KeyboardEvent::KEY_F13;
		case Qt::Key_F14:
			return KeyboardEvent::KEY_F14;
		case Qt::Key_F15:
			return KeyboardEvent::KEY_F15;
		case Qt::Key_Kana_Lock:
			return KeyboardEvent::KEY_KANA;
		case Qt::Key_yen:
			return KeyboardEvent::KEY_YEN;
		case Qt::Key_Colon:
			return KeyboardEvent::KEY_COLON;
		case Qt::Key_Underscore:
			return KeyboardEvent::KEY_UNDERLINE;
		case Qt::Key_Kanji:
			return KeyboardEvent::KEY_KANJI;
		case Qt::Key_Stop:
			return KeyboardEvent::KEY_STOP;
		case Qt::Key_Calculator:
			return KeyboardEvent::KEY_CALCULATOR;
		case Qt::Key_MediaStop:
			return KeyboardEvent::KEY_MEDIASTOP;
		case Qt::Key_VolumeDown:
			return KeyboardEvent::KEY_VOLUMEDOWN;
		case Qt::Key_VolumeUp:
			return KeyboardEvent::KEY_VOLUMEUP;
		case Qt::Key_VolumeMute:
			return KeyboardEvent::KEY_MUTE;
		case Qt::Key_MediaNext:
			return KeyboardEvent::KEY_NEXTTRACK;
		case Qt::Key_MediaLast:
			return KeyboardEvent::KEY_PREVTRACK;
		case Qt::Key_HomePage:
			return KeyboardEvent::KEY_WEBHOME;
		case Qt::Key_division:
			return KeyboardEvent::KEY_DIVIDE;
		case Qt::Key_SysReq:
			return KeyboardEvent::KEY_SYSRQ;
		case Qt::Key_Pause:
			return KeyboardEvent::KEY_PAUSE;
		case Qt::Key_Home:
			return KeyboardEvent::KEY_HOME;
		case Qt::Key_Up:
			return KeyboardEvent::KEY_UP;
		case Qt::Key_PageUp:
			return KeyboardEvent::KEY_PGUP;
		case Qt::Key_Left:
			return KeyboardEvent::KEY_LEFT;
		case Qt::Key_Right:
			return KeyboardEvent::KEY_RIGHT;
		case Qt::Key_End:
			return KeyboardEvent::KEY_END;
		case Qt::Key_Down:
			return KeyboardEvent::KEY_DOWN;
		case Qt::Key_PageDown:
			return KeyboardEvent::KEY_PGDOWN;
		case Qt::Key_Insert:
			return KeyboardEvent::KEY_INSERT;
		case Qt::Key_Delete:
			return KeyboardEvent::KEY_DELETE;
		case Qt::Key_Meta:
			return KeyboardEvent::KEY_LWIN;
		case Qt::Key_PowerOff:
			return KeyboardEvent::KEY_POWER;
		case Qt::Key_Sleep:
			return KeyboardEvent::KEY_SLEEP;
		case Qt::Key_WakeUp:
			return KeyboardEvent::KEY_WAKE;
		case Qt::Key_Search:
			return KeyboardEvent::KEY_WEBSEARCH;
		case Qt::Key_Favorites:
			return KeyboardEvent::KEY_WEBFAVORITES;
		case Qt::Key_Refresh:
			return KeyboardEvent::KEY_WEBREFRESH;
		case Qt::Key_Forward:
			return KeyboardEvent::KEY_WEBFORWARD;
		case Qt::Key_Back:
			return KeyboardEvent::KEY_WEBBACK;
		case Qt::Key_LaunchMail:
			return KeyboardEvent::KEY_MAIL;
		case Qt::Key_LaunchMedia:
			return KeyboardEvent::KEY_MEDIASELECT;

		default:
			return KeyboardEvent::KEY_UNASSIGNED;
		}
	}
}


} // namespace Armed
