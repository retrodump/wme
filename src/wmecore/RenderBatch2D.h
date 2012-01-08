// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRenderBatch2D_H__
#define __WmeRenderBatch2D_H__


namespace Wme
{
	class Viewport;
	class Transform2D;

	class WmeDllExport RenderBatch2D : public Ogre::Renderable
	{
	public:
		RenderBatch2D();
		virtual ~RenderBatch2D();

		void SetVertices(Viewport* viewport, Vertex2D* vertexData, size_t numVerts, const Transform2D& transform, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType);
		void SetVertices(Viewport* viewport, Vertex2DTex* vertexData, size_t numVerts, const Transform2D& transform, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType);

		// Ogre::Renderable
		virtual const Ogre::MaterialPtr& getMaterial(void) const;
		virtual void getRenderOperation(Ogre::RenderOperation& op);
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
		const Ogre::LightList& getLights(void) const;

	private:

		typedef enum Type
		{
			RENDER_BATCH_COLOR,
			RENDER_BATCH_TEXTURE
		};

		typedef struct
		{
			Ogre::Vector3 pos;
			Ogre::RGBA color;
		} VertexColor;

		typedef struct
		{
			Ogre::Vector3 pos;
			Ogre::RGBA color;
			Ogre::Vector2 texCoord;
		} VertexTexture;

		void InitHWBuffer(Type type, size_t numVerts, Ogre::RenderOperation::OperationType operType);
		Type m_CurrentType;
		size_t m_CurrentBufferSize;
		size_t m_CurrentNumVerts;

		Ogre::HardwareVertexBufferSharedPtr m_HardwareBuffer;
		Ogre::RenderOperation m_RenderOperation;
		bool m_HWBufferInitialized;

		Ogre::MaterialPtr m_Material;


	};
}

#endif // __WmeRenderBatch2D_H__
