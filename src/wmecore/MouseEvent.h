// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMouseEvent_H__
#define __WmeMouseEvent_H__


namespace Wme
{
	class Viewport;

	class WmeDllExport MouseEvent
	{
	public:
		enum MouseButton
		{
			BUTTON_NONE = 0x00,
			BUTTON_LEFT = 0x01,
			BUTTON_RIGHT = 0x02,
			BUTTON_MIDDLE = 0x04
		};

		enum EventType
		{
			EVENT_NONE,
			EVENT_PRESS,
			EVENT_RELEASE,
			EVENT_DOUBLE_CLICK,
			EVENT_WHEEL_UP,
			EVENT_WHEEL_DOWN
		};

		MouseEvent(EventType type, MouseButton button, int posX, int posY);
		~MouseEvent();


		EventType GetType() const { return m_Type; }
		MouseButton GetButton() const { return m_Button; }
		int GetPosX() const { return m_PosX; }
		int GetPosY() const { return m_PosY; }

		void SetPos(int x, int y);

		void TranslateToViewport(Viewport* viewport);

	private:
		EventType m_Type;
		MouseButton m_Button;
		int m_PosX;
		int m_PosY;

	};
}

#endif // __WmeMouseEvent_H__
