// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "GuiStage.h"
#include "Viewport.h"
#include "ViewportLayout.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
GuiStage::GuiStage()
{
}

//////////////////////////////////////////////////////////////////////////
GuiStage::~GuiStage()
{
}

//////////////////////////////////////////////////////////////////////////
void GuiStage::Create()
{
	Stage::Create();
}

//////////////////////////////////////////////////////////////////////////
void GuiStage::OnViewportCreated(Viewport* viewport)
{
}

//////////////////////////////////////////////////////////////////////////
void GuiStage::Render(Viewport* viewport, Camera* camera)
{
	Stage::Render(viewport, camera);
}

//////////////////////////////////////////////////////////////////////////
void GuiStage::Update()
{
	Stage::Update();
}

//////////////////////////////////////////////////////////////////////////
bool GuiStage::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	bool res = Stage::HandleMouseEvent(viewport, event);

	MousePickResult result;	
	if (viewport->GetParentLayout()->GetObjectAt(event.GetPosX(), event.GetPosY(), result))
	{
		if (result.Object->HandleMouseEvent(viewport, event)) res = true;
	}
	return res;
}


} // namespace Wme
