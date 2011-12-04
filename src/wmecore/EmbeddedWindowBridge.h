// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEmbeddedWindowBridge_H__
#define __WmeEmbeddedWindowBridge_H__


namespace Wme
{
	class WmeDllExport EmbeddedWindowBridge
	{
	public:
		EmbeddedWindowBridge();
		virtual ~EmbeddedWindowBridge();

		virtual AnsiString GetWindowId() const = 0;
		virtual void GetWindowSize(size_t& width, size_t& height) = 0;

		virtual void GetMousePos(int& x, int& y) const = 0;
		virtual void SetMousePos(int x, int y) = 0;
		virtual bool IsShiftDown() const = 0;
		virtual bool IsControlDown() const = 0;
		virtual bool IsAltDown() const = 0;

	};
}

#endif // __WmeEmbeddedWindowBridge_H__