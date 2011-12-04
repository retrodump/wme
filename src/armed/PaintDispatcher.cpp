// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PaintDispatcher.h"
#include "Game.h"

using namespace Wme;

namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PaintDispatcher::PaintDispatcher(QWidget* parent) : QWidget(parent)
{
}

//////////////////////////////////////////////////////////////////////////
PaintDispatcher::~PaintDispatcher()
{
}

//////////////////////////////////////////////////////////////////////////
void PaintDispatcher::paintEvent(QPaintEvent* event)
{
	Game::GetInstance()->ProcessFrame();
}


} // namespace Armed
