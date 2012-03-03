// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Scene2DBase.h"
#include "Viewport.h"
#include "Canvas2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Scene2DBase::Scene2DBase()
{
	m_Viewport = NULL;
	m_Canvas = NULL;
}

//////////////////////////////////////////////////////////////////////////
Scene2DBase::~Scene2DBase()
{
	SAFE_DELETE(m_Canvas);
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Create()
{
	Scene::Create();

	m_Canvas = new Canvas2D(GetUniqueMovableName("Canvas2D", L"Canvas2D"));
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Update()
{
	Scene::Update();
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::Shutdown()
{
	Scene::Shutdown();
}

//////////////////////////////////////////////////////////////////////////
void Scene2DBase::SetViewport(Viewport* viewport)
{
	if (m_Canvas) m_Canvas->SetViewport(viewport);
}


} // namespace Wme
