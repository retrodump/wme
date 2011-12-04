// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeStageEditor_H__
#define __WmeStageEditor_H__


#include "Object.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"


namespace Wme
{
	class Stage;
	class Viewport;

	class WmeDllExport StageEditor : public Object
	{
	public:
		StageEditor(Stage* stage);
		virtual ~StageEditor();

		Stage* GetStage() { return m_Stage; }

		virtual void Create() = 0;
		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event) = 0;
		virtual bool HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event) = 0;
		virtual void Update() = 0;
		virtual void PreRender(Viewport* viewport) = 0;

		virtual bool Is3DEditor() { return false; }

	protected:
		Stage* m_Stage;
	};
}

#endif // __WmeStageEditor_H__