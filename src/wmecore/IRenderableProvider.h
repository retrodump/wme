// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeIRenderableProvider_H__
#define __WmeIRenderableProvider_H__


namespace Wme
{
	class Ogre::Renderable;

	class WmeDllExport IRenderableProvider
	{
	public:
		virtual ~IRenderableProvider() {}

		typedef std::list<Ogre::Renderable*> RenderableList;

		virtual void GetRenderables(RenderableList& renderableList) = 0;
	};
}

#endif // __WmeIRenderableProvider_H__
