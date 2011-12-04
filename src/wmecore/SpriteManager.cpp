// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SpriteManager.h"
#include "SpriteTexture.h"
#include "SpriteSubFrame.h"
#include "Viewport.h"
#include "ActiveSpot.h"
#include "GameScriptable.h" // temp


namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
SpriteManager::SpriteManager()
{
	m_Initialized = false;
}

//////////////////////////////////////////////////////////////////////////
SpriteManager::~SpriteManager()
{
	DestroySpriteHWBuffer();
}

//////////////////////////////////////////////////////////////////////////
bool SpriteManager::Initialize()
{
	CreateSpriteHWBuffer();
	m_Initialized = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void SpriteManager::xxDisplaySprite(int targetX, int targetY, const SpriteSubFrame* subFrame, const SpriteDrawingParams& params)
{
	// ----- no longer used -----

	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	xxSetupRenderSystem();

	Ogre::RenderOperation* renderOp = xxRenderSprite(targetX, targetY, subFrame, params);

	rs->_setTexture(0, true, subFrame->GetTexture()->GetTexture());
	rs->_render(*renderOp);
}

//////////////////////////////////////////////////////////////////////////
void SpriteManager::PrepareSpriteVertices(SpriteVertex* verts, int targetX, int targetY, const SpriteSubFrame* subFrame, const SpriteDrawingParams& params)
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	const SpriteTexture* texture = subFrame->GetTexture();


	// adjust target position according to base point
	targetX -= (int)((float)texture->GetWidth()  * params.Scale.x * subFrame->GetBasePoint().x);
	targetY -= (int)((float)texture->GetHeight() * params.Scale.y * subFrame->GetBasePoint().y);


	// vertices
	float posLeft, posRight, posBottom, posTop;
	posLeft = (float)targetX;
	posRight = (float)(targetX + (float)texture->GetWidth() * params.Scale.x);
	posTop = (float)targetY;
	posBottom = (float)(targetY + (float)texture->GetHeight() * params.Scale.y);

	verts[0].pos = Ogre::Vector3(posLeft,  posBottom, -1.0f); // lower left
	verts[1].pos = Ogre::Vector3(posLeft,  posTop,    -1.0f); // top left
	verts[2].pos = Ogre::Vector3(posRight, posBottom, -1.0f); // bottom right
	verts[3].pos = Ogre::Vector3(posRight, posTop,    -1.0f); // top right

	SpritePlacement spritePlacement;

	// rotation
	if (params.Rotation != 0.0)
	{
		Ogre::Vector2 centerPoint(posLeft + (posRight - posLeft) / 2, posTop + (posBottom - posTop) / 2);
		params.Viewport->RotateVertices(verts, 4, centerPoint, params.Rotation);

		spritePlacement.Rotation = params.Rotation;
		spritePlacement.CenterPoint = centerPoint;

		if (!params.AbsolutePos)
		{
			spritePlacement.CenterPoint.x -= params.Viewport->GetScrollOffsetX();
			spritePlacement.CenterPoint.y -= params.Viewport->GetScrollOffsetY();
		}
	}

	// store parameters for mouse picking
	for (int i = 0; i < 4; i++)
	{
		spritePlacement.Vertices[i].x = verts[i].pos.x;
		spritePlacement.Vertices[i].y = verts[i].pos.y;
	}


	// transform to ogre screen space
	params.Viewport->TransformVertices(verts, 4, params.AbsolutePos);


	// texture coordinates
	float texLeft, texRight, texBottom, texTop;
	texLeft = texTop = 0.0;
	texRight = texture->GetTexCoordU();
	texBottom = texture->GetTexCoordV();

	if (params.MirrorX)
	{
		float temp = texTop;
		texTop = texBottom;
		texBottom = temp;
	}

	if (params.MirrorY)
	{
		float temp = texLeft;
		texLeft = texRight;
		texRight = temp;
	}


	// color
	Ogre::uint32 color;
	rs->convertColourValue(params.Color, &color); // convert to platform native format

	verts[0].color = verts[1].color = verts[2].color = verts[3].color = color;


	// texture coordinates
	verts[0].texCoord.x = texLeft;	verts[0].texCoord.y = texBottom; // lower left
	verts[1].texCoord.x = texLeft;	verts[1].texCoord.y = texTop;    // top left
	verts[2].texCoord.x = texRight; verts[2].texCoord.y = texBottom; // bottom right
	verts[3].texCoord.x = texRight; verts[3].texCoord.y = texTop;    // top right


	if (params.Parent)
		params.Viewport->AddActiveSpot(new ActiveSpotSprite(params.Viewport, targetX, targetY, subFrame, spritePlacement, params));

}

//////////////////////////////////////////////////////////////////////////
Ogre::RenderOperation* SpriteManager::RenderSubFrame(const SpriteSubFrame* subFrame)
{
	if (!m_Initialized) Initialize();

	// TODO - dynamic vertex buffer
	SpriteVertex* bufVerts = reinterpret_cast<SpriteVertex*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	const SpriteVertex* verts = subFrame->GetVertices();

	for (int i = 0; i < 4; i++)
	{
		bufVerts[i] = verts[i];
	}

	m_HardwareBuffer->unlock();

	m_RenderOperation.vertexData->vertexStart = 0;
	m_RenderOperation.vertexData->vertexCount = 4;

	return &m_RenderOperation;
}

//////////////////////////////////////////////////////////////////////////
Ogre::RenderOperation* SpriteManager::xxRenderSprite(int targetX, int targetY, const SpriteSubFrame* subFrame, const SpriteDrawingParams& params)
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	const SpriteTexture* texture = subFrame->GetTexture();


	// adjust target position according to base point
	targetX -= (int)((float)texture->GetWidth()  * params.Scale.x * subFrame->GetBasePoint().x);
	targetY -= (int)((float)texture->GetHeight() * params.Scale.y * subFrame->GetBasePoint().y);


	// lock hw buffer and fill sprite vertices
	// TODO - dynamic vertex buffer
	SpriteVertex* verts = reinterpret_cast<SpriteVertex*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	
	// vertices
	float posLeft, posRight, posBottom, posTop;
	posLeft = (float)targetX;
	posRight = (float)(targetX + (float)texture->GetWidth() * params.Scale.x);
	posTop = (float)targetY;
	posBottom = (float)(targetY + (float)texture->GetHeight() * params.Scale.y);

	verts[0].pos = Ogre::Vector3(posLeft,  posBottom, -1.0f); // lower left
	verts[1].pos = Ogre::Vector3(posLeft,  posTop,    -1.0f); // top left
	verts[2].pos = Ogre::Vector3(posRight, posBottom, -1.0f); // bottom right
	verts[3].pos = Ogre::Vector3(posRight, posTop,    -1.0f); // top right

	SpritePlacement spritePlacement;

	// rotation
	if (params.Rotation != 0.0)
	{
		Ogre::Vector2 centerPoint(posLeft + (posRight - posLeft) / 2, posTop + (posBottom - posTop) / 2);
		params.Viewport->RotateVertices(verts, 4, centerPoint, params.Rotation);

		spritePlacement.Rotation = params.Rotation;
		spritePlacement.CenterPoint = centerPoint;

		if (!params.AbsolutePos)
		{
			spritePlacement.CenterPoint.x -= params.Viewport->GetScrollOffsetX();
			spritePlacement.CenterPoint.y -= params.Viewport->GetScrollOffsetY();
		}
	}

	// store parameters for mouse picking
	for (int i = 0; i < 4; i++)
	{
		spritePlacement.Vertices[i].x = verts[i].pos.x;
		spritePlacement.Vertices[i].y = verts[i].pos.y;
	}


	// transform to ogre screen space
	params.Viewport->TransformVertices(verts, 4, params.AbsolutePos);


	// texture coordinates
	float texLeft, texRight, texBottom, texTop;
	texLeft = texTop = 0.0;
	texRight = texture->GetTexCoordU();
	texBottom = texture->GetTexCoordV();

	if (params.MirrorX)
	{
		float temp = texTop;
		texTop = texBottom;
		texBottom = temp;
	}

	if (params.MirrorY)
	{
		float temp = texLeft;
		texLeft = texRight;
		texRight = temp;
	}

		
	// color
	Ogre::uint32 color;
	rs->convertColourValue(params.Color, &color); // convert to platform native format

	verts[0].color = verts[1].color = verts[2].color = verts[3].color = color;


	// texture coordinates
	verts[0].texCoord.x = texLeft;	verts[0].texCoord.y = texBottom; // lower left
	verts[1].texCoord.x = texLeft;	verts[1].texCoord.y = texTop;    // top left
	verts[2].texCoord.x = texRight; verts[2].texCoord.y = texBottom; // bottom right
	verts[3].texCoord.x = texRight; verts[3].texCoord.y = texTop;    // top right


	m_HardwareBuffer->unlock();

	m_RenderOperation.vertexData->vertexStart = 0;
	m_RenderOperation.vertexData->vertexCount = 4;


	if (params.Parent)
		params.Viewport->AddActiveSpot(new ActiveSpotSprite(params.Viewport, targetX, targetY, subFrame, spritePlacement, params));

	return &m_RenderOperation;
}

//////////////////////////////////////////////////////////////////////////
void SpriteManager::CreateSpriteHWBuffer()
{
	m_RenderOperation.useIndexes = false;
	m_RenderOperation.vertexData = new Ogre::VertexData();
	Ogre::VertexDeclaration* decl = m_RenderOperation.vertexData->vertexDeclaration;

	size_t offset = 0;
	
	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	
	decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

	decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

	m_HardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		decl->getVertexSize(0),
		4,
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	
	m_RenderOperation.vertexData->vertexBufferBinding->setBinding(0, m_HardwareBuffer);

	m_RenderOperation.operationType = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
}

//////////////////////////////////////////////////////////////////////////
void SpriteManager::DestroySpriteHWBuffer()
{
	SAFE_DELETE(m_RenderOperation.vertexData);
	if (!m_HardwareBuffer.isNull()) m_HardwareBuffer.setNull();
}

//////////////////////////////////////////////////////////////////////////
void SpriteManager::xxSetupRenderSystem()
{
	// ----- no longer used -----


	Ogre::LayerBlendModeEx colorBlendMode;
	Ogre::LayerBlendModeEx alphaBlendMode;
	Ogre::TextureUnitState::UVWAddressingMode uvwAddressMode;

	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	colorBlendMode.blendType = Ogre::LBT_COLOUR;
	colorBlendMode.source1 = Ogre::LBS_TEXTURE;
	colorBlendMode.source2 = Ogre::LBS_DIFFUSE;
	colorBlendMode.operation = Ogre::LBX_MODULATE;

	alphaBlendMode.blendType = Ogre::LBT_ALPHA;
	alphaBlendMode.source1 = Ogre::LBS_TEXTURE;
	alphaBlendMode.source2 = Ogre::LBS_DIFFUSE;
	alphaBlendMode.operation = Ogre::LBX_MODULATE;

	uvwAddressMode.u = Ogre::TextureUnitState::TAM_CLAMP;
	uvwAddressMode.v = Ogre::TextureUnitState::TAM_CLAMP;
	uvwAddressMode.w = Ogre::TextureUnitState::TAM_CLAMP;

	rs->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	rs->_setTextureCoordSet(0, 0);
	rs->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	rs->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_LINEAR);
	rs->_setTextureBlendMode(0, colorBlendMode);
	rs->_setTextureBlendMode(0, alphaBlendMode);
	rs->_setTextureAddressingMode(0, uvwAddressMode);
	rs->_disableTextureUnitsFrom(1);
	rs->setLightingEnabled(false);
	rs->_setFog(Ogre::FOG_NONE);
	rs->_setCullingMode(Ogre::CULL_NONE);
	rs->_setDepthBufferParams(false, false);
	rs->_setColourBufferWriteEnabled(true, true, true, false);
	rs->setShadingType(Ogre::SO_GOURAUD);
	rs->_setPolygonMode(Ogre::PM_SOLID);
	rs->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	rs->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
	rs->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
	rs->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0, false);
}

} // namespace Wme
