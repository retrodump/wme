// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Scene.h"
#include "Game.h"
#include "Renderer.h"
#include "Viewport.h"
#include "ElementCollection.h"
#include "SpriteSubFrame.h"
#include "TextElement.h"
#include "XmlUtil.h"
#include "StringUtil.h"
#include "OgreSceneLoader.h"
#include "InputManager.h"
#include "MainWindow.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
}

//////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
}

//////////////////////////////////////////////////////////////////////////
bool Scene::LoadFromXml(TiXmlElement* rootNode)
{
	return InteractiveObject::LoadFromXml(rootNode);
}

//////////////////////////////////////////////////////////////////////////
bool Scene::SaveToXml(TiXmlElement* rootNode)
{
	return InteractiveObject::SaveToXml(rootNode);
}


} // namespace Wme
