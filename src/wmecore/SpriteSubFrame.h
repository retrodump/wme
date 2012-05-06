// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSpriteSubFrame_H__
#define __WmeSpriteSubFrame_H__

#include "OgreRenderable.h"
#include "ScriptableObject.h"
#include "Sprite.h"
#include "SpriteFrame.h"
#include "IRenderableProvider.h"

namespace Wme
{

	class SpriteTexture;

	class WmeDllExport SpriteSubFrame : public ScriptableObject, public Ogre::Renderable, public IRenderableProvider
	{
		friend class SpriteFrame;
	public:
		SpriteSubFrame(SpriteFrame* frame);
		virtual ~SpriteSubFrame();


		void SetBasePoint(const Ogre::Vector2 basePoint);
		const Ogre::Vector2& GetBasePoint() const;

		bool SetTexture(const WideString& fileName, Ogre::ColourValue colorKey = Ogre::ColourValue::ZERO);
		const SpriteTexture* GetTexture() const { return m_Texture; }

		void Display(ElementCollection* elementCol, int targetX, int targetY, const SpriteDrawingParams& params);

		bool IsTransparentAt(int posX, int posY) const;

		int GetWidth() const;
		int GetHeight() const;

		void GetBoundingRect(Rect& rect) const;

		SpriteFrame* GetFrame() const { return m_Frame; }
		Sprite* GetSprite() const { return m_Frame ? m_Frame->GetSprite() : NULL; }

		bool IsDirty() const { return m_IsDirty; }
		void SetDirty(bool isDirty);

		bool GetMirrorHorizontal() const { return m_MirrorHorizontal; }
		void SetMirrorHorizontal(bool val);

		bool GetMirrorVertical() const { return m_MirrorVertical; }
		void SetMirrorVertical(bool val);

		const SpriteVertex* GetVertices() const { return m_Vertices; }

		// ScriptableObject
		RTTI(SpriteSubFrame);
		virtual WideString ConvertToString() { return L"SpriteSubFrame object"; }
		virtual WideString GetTypeName() const { return L"SpriteSubFrame"; }

        // DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* wmeSubFrameNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);

		// IRenderableProvider implementation
		virtual void GetRenderables(IRenderableProvider::RenderableList& renderableList);

		// Ogre::Renderable implementation
		virtual const Ogre::MaterialPtr& getMaterial(void) const;
		virtual void getRenderOperation(Ogre::RenderOperation& op);
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
		const Ogre::LightList& getLights(void) const;

		virtual bool preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rs);

	private:
		bool LoadFromImage(const WideString& fileName);

		Ogre::Vector2 m_BasePoint;		
		SpriteTexture* m_Texture;

		bool m_MirrorHorizontal;
		bool m_MirrorVertical;
		Ogre::ColourValue m_ColorKey;
		
		Ogre::MaterialPtr m_EmptyMaterial;

		SpriteVertex m_Vertices[4];

		SpriteFrame* m_Frame;
		bool m_IsDirty;
	};
}


#endif // __WmeSpriteSubFrame_H__