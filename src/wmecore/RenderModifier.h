// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRenderModifier_H__
#define __WmeRenderModifier_H__


#include "IRenderableProvider.h"
#include "Rect.h"


namespace Wme
{
	class Viewport;

	class WmeDllExport RenderModifier : public IRenderableProvider, public Ogre::Renderable
	{
	public:
		RenderModifier();
		~RenderModifier();

		void SetClipping(bool clip, Viewport* viewport, const Rect rect = Rect::EMPTY_RECT);

		// IRenderableProvider implementation
		virtual void GetRenderables(IRenderableProvider::RenderableList& renderableList);

		// Ogre::Renderable implementation
		virtual const Ogre::MaterialPtr& getMaterial(void) const;
		virtual void getRenderOperation(Ogre::RenderOperation& op);
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
		const Ogre::LightList& getLights(void) const;
		bool preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rsys);

	private:
		Ogre::MaterialPtr m_Material;

		bool m_Clip;
		Viewport* m_Viewport;
		Rect m_ClippingRect;

	};
}


#endif // __WmeRenderModifier_H__
