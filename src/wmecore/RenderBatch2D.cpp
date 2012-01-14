// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "RenderBatch2D.h"
#include "Transform2D.h"
#include "Viewport.h"
#include "BoundingRect.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
RenderBatch2D::RenderBatch2D()
{
	m_CurrentType = RENDER_BATCH_COLOR;
	m_CurrentBufferSize = 0;
	m_CurrentNumVerts = 0;
	m_HWBufferInitialized = false;

	// Ogre::Renderable
	mUseIdentityProjection = true;
	mUseIdentityView = true;
	mPolygonModeOverrideable = false;
}

//////////////////////////////////////////////////////////////////////////
RenderBatch2D::~RenderBatch2D()
{
	SAFE_DELETE(m_RenderOperation.vertexData);
	if (!m_HardwareBuffer.isNull()) m_HardwareBuffer.setNull();
}

//////////////////////////////////////////////////////////////////////////
void RenderBatch2D::SetVertices(Viewport* viewport, Vertex2D* vertexData, size_t numVerts, const Transform2D& transform, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType, BoundingRect& boundingRect)
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	InitHWBuffer(RENDER_BATCH_COLOR, numVerts, operType);

	// copy vertices to hw buffer
	VertexColor* verts = reinterpret_cast<VertexColor*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	for (size_t i = 0; i < numVerts; i++)
	{
		Ogre::uint32 color;
		rs->convertColourValue(vertexData[i].color, &color); // convert color to platform native format

		Ogre::Vector2 pos = transform * vertexData[i].pos;
		boundingRect.AddPoint(pos);

		verts[i].pos = Ogre::Vector3(pos.x, pos.y, -1.0f);
		verts[i].color = color;
	}

	viewport->Transform2DVertices(reinterpret_cast<byte*>(verts), numVerts, sizeof(VertexColor));

	m_HardwareBuffer->unlock();

	m_CurrentNumVerts = numVerts;
	m_Material = material;
}

//////////////////////////////////////////////////////////////////////////
void RenderBatch2D::SetVertices(Viewport* viewport, Vertex2DTex* vertexData, size_t numVerts, const Transform2D& transform, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType, BoundingRect& boundingRect)
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	InitHWBuffer(RENDER_BATCH_TEXTURE, numVerts, operType);

	// copy vertices to hw buffer
	VertexTexture* verts = reinterpret_cast<VertexTexture*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	for (size_t i = 0; i < numVerts; i++)
	{
		Ogre::uint32 color;
		rs->convertColourValue(vertexData[i].color, &color); // convert color to platform native format

		Ogre::Vector2 pos = transform * vertexData[i].pos;
		boundingRect.AddPoint(pos);

		verts[i].pos = Ogre::Vector3(pos.x, pos.y, -1.0f);
		verts[i].color = color;
		verts[i].texCoord = vertexData[i].texCoord;
	}

	viewport->Transform2DVertices(reinterpret_cast<byte*>(verts), numVerts, sizeof(VertexTexture));

	m_HardwareBuffer->unlock();

	m_CurrentNumVerts = numVerts;
	m_Material = material;
}

//////////////////////////////////////////////////////////////////////////
void RenderBatch2D::InitHWBuffer(Type type, size_t numVerts, Ogre::RenderOperation::OperationType operType)
{
	if (!m_HWBufferInitialized || type != m_CurrentType)
	{
		m_CurrentType = type;
		m_CurrentBufferSize = 0;

		// create buffer
		if (!m_HWBufferInitialized)
		{
			m_RenderOperation.useIndexes = false;
			m_RenderOperation.vertexData = new Ogre::VertexData();
			m_HWBufferInitialized = true;
		}

		// create vertex declaration
		Ogre::VertexDeclaration* decl = m_RenderOperation.vertexData->vertexDeclaration;

		size_t offset = 0;
		decl->removeAllElements();

		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

		if (type == RENDER_BATCH_TEXTURE)
		{
			decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
		}		
	}

	if (numVerts == 0) numVerts = 1;

	// resize buffer when needed
	if (numVerts > m_CurrentBufferSize)
	{
		m_CurrentBufferSize = numVerts;

		m_HardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			m_RenderOperation.vertexData->vertexDeclaration->getVertexSize(0),
			m_CurrentBufferSize,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		m_RenderOperation.vertexData->vertexBufferBinding->setBinding(0, m_HardwareBuffer);		
	}

	// store operation type (triangle list, trinagle strips, line list...)
	m_RenderOperation.operationType = operType;
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Renderable
//////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr& RenderBatch2D::getMaterial(void) const
{
	return m_Material;
}

//////////////////////////////////////////////////////////////////////////
void RenderBatch2D::getRenderOperation(Ogre::RenderOperation& op)
{
	op = m_RenderOperation;
	op.vertexData->vertexStart = 0;
	op.vertexData->vertexCount = m_CurrentNumVerts;
}

//////////////////////////////////////////////////////////////////////////
void RenderBatch2D::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::LightList& RenderBatch2D::getLights(void) const
{
	// N/A
	static Ogre::LightList ll;
	return ll;
}


} // namespace Wme
