// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SpriteSubFrame.h"
#include "SpriteManager.h"
#include "SpriteTexture.h"
#include "ElementCollection.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SpriteSubFrame::SpriteSubFrame(SpriteFrame* frame)
{
	m_Frame = frame;
	m_IsDirty  = false;

	m_BasePoint = Ogre::Vector2(0.0, 0.0);
	m_Texture = NULL;

	m_MirrorHorizontal = m_MirrorVertical = false;
	m_ColorKey = Ogre::ColourValue::ZERO;

	mUseIdentityProjection = true;
	mUseIdentityView = true;
	mPolygonModeOverrideable = false;
}

//////////////////////////////////////////////////////////////////////////
SpriteSubFrame::~SpriteSubFrame()
{
	SAFE_DELETE(m_Texture);
}

//////////////////////////////////////////////////////////////////////////
int SpriteSubFrame::GetWidth() const
{
	if (m_Texture) return m_Texture->GetWidth();
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
int SpriteSubFrame::GetHeight() const
{
	if (m_Texture) return m_Texture->GetHeight();
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::GetBoundingRect(Rect& rect) const
{
	rect.x = -m_BasePoint.x;
	rect.y = -m_BasePoint.y;
	rect.width = (float)GetWidth();
	rect.height = (float)GetHeight();
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::SetDirty(bool isDirty)
{
	m_IsDirty = isDirty;
	if (isDirty && m_Frame) m_Frame->SetDirty(true);
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::Display(ElementCollection* elementCol, int targetX, int targetY, const SpriteDrawingParams& params)
{
	SpriteDrawingParams newParams = params;
	newParams.MirrorX = m_MirrorVertical;
	newParams.MirrorY = m_MirrorHorizontal;

	Game::GetInstance()->GetSpriteMgr()->PrepareSpriteVertices(m_Vertices, targetX, targetY, this, newParams);

	elementCol->AddElement(this);
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::SetBasePoint(const Ogre::Vector2 basePoint)
{
	if (basePoint != m_BasePoint)
	{
		m_BasePoint = basePoint;
		SetDirty(true);
	}
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector2& SpriteSubFrame::GetBasePoint() const
{
	return m_BasePoint;
}

//////////////////////////////////////////////////////////////////////////
bool SpriteSubFrame::SetTexture(const WideString& fileName, Ogre::ColourValue colorKey)
{
	SAFE_DELETE(m_Texture);
	SetDirty(true);

	m_Texture = new SpriteTexture();
	if (m_Texture->SetTexture(fileName, colorKey)) return true;
	else
	{
		SAFE_DELETE(m_Texture);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::SetMirrorHorizontal(bool val)
{
	if (val != m_MirrorHorizontal)
	{
		m_MirrorHorizontal = val;
		SetDirty(true);
	}
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::SetMirrorVertical(bool val)
{
	if (val != m_MirrorVertical)
	{
		m_MirrorVertical = val;
		SetDirty(true);
	}
}

//////////////////////////////////////////////////////////////////////////
bool SpriteSubFrame::IsTransparentAt(int posX, int posY) const
{
	if (m_MirrorHorizontal) posX = m_Texture->GetWidth() - posX;
	if (m_MirrorVertical) posY = m_Texture->GetHeight() - posY;

	return m_Texture->IsTransparentAt(posX, posY);
}

//////////////////////////////////////////////////////////////////////////
bool SpriteSubFrame::LoadFromXml(TiXmlElement* rootNode)
{
	WideString imageName = L"";
	Ogre::ColourValue colorKey = Ogre::ColourValue::ZERO;
	Ogre::Vector2 basePoint = Ogre::Vector2::ZERO;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Image")
		{
			imageName = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "BasePoint")
		{
			basePoint = XmlUtil::GetVector2(elem);
		}
		else if (elem->ValueStr() == "ColorKey")
		{
			colorKey = XmlUtil::GetColor(elem);
		}
		else if (elem->ValueStr() == "MirrorVertical")
		{
			m_MirrorVertical = XmlUtil::TextToBool(elem);
		}
		else if (elem->ValueStr() == "MirrorHorizontal")
		{
			m_MirrorHorizontal = XmlUtil::TextToBool(elem);
		}
	}

	SetDirty(true);

	if (imageName.empty() || !SetTexture(imageName, colorKey)) return false;
	else
	{
		m_BasePoint.x = basePoint.x / GetWidth();
		m_BasePoint.y = basePoint.y / GetHeight();

		return true;
	}
}

//////////////////////////////////////////////////////////////////////////
bool SpriteSubFrame::SaveToXml(TiXmlElement* rootNode)
{
	if (!m_Texture) return false;

	TiXmlElement* elem;

	TiXmlElement* subFrameNode = XmlUtil::AddElem("SubFrame", rootNode);

	elem = XmlUtil::AddElem("Image", subFrameNode);
	XmlUtil::SetFileName(elem, m_Texture->GetFileName());

	elem = XmlUtil::AddElem("BasePoint", subFrameNode);
	XmlUtil::SetVector2(elem, Ogre::Vector2(m_BasePoint.x * m_Texture->GetWidth(), m_BasePoint.y * m_Texture->GetHeight()));

	elem = XmlUtil::AddElem("ColorKey", subFrameNode);
	XmlUtil::SetColor(elem, m_Texture->GetColorKey());

	elem = XmlUtil::AddElem("MirrorVertical", subFrameNode);
	XmlUtil::SetText(elem, m_MirrorVertical);

	elem = XmlUtil::AddElem("MirrorHorizontal", subFrameNode);
	XmlUtil::SetText(elem, m_MirrorHorizontal);


	return true;
}

//////////////////////////////////////////////////////////////////////////
bool SpriteSubFrame::LoadFromImage(const WideString& fileName)
{
	return SetTexture(fileName);
}

//////////////////////////////////////////////////////////////////////////
// IRenderableProvider
//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::GetRenderables(IRenderableProvider::RenderableList& renderableList)
{
	renderableList.push_back(this);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Renderable
//////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr& SpriteSubFrame::getMaterial(void) const
{
	if (m_Texture) return m_Texture->GetMaterial();
	else return m_EmptyMaterial;
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::getRenderOperation(Ogre::RenderOperation& op)
{
	Ogre::RenderOperation* renderOp = Game::GetInstance()->GetSpriteMgr()->RenderSubFrame(this);
	op = *renderOp;
}

//////////////////////////////////////////////////////////////////////////
void SpriteSubFrame::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::LightList& SpriteSubFrame::getLights(void) const
{
	// N/A
	static Ogre::LightList ll;
	return ll;
}


//////////////////////////////////////////////////////////////////////////
bool SpriteSubFrame::preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rs)
{
/*
	Ogre::LayerBlendModeEx colorBlendMode;
	Ogre::LayerBlendModeEx alphaBlendMode;
	Ogre::TextureUnitState::UVWAddressingMode uvwAddressMode;

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
	//rs->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0, false);
	rs->_setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 0, true);
*/
	return true;
}


} // namespace Wme
