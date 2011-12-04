// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "FontGlyphCache.h"
#include "Game.h"
#include "OgreBitwise.h"


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment (lib, "freetype2311_D.lib")
#	else
#		pragma comment (lib, "freetype2311.lib")
#	endif
#endif


namespace Wme
{


int FontGlyphCache::s_TextureCounter = 0;

//////////////////////////////////////////////////////////////////////////
FontGlyphCache::FontGlyphCache()
{
	m_GridWidth = m_GridHeight = 0;
	m_CurrentPage = -1;
	m_CurrentPosX = m_CurrentPosY = 0;
	m_TextureWidth = 0;
	m_CurrentRowHeight = 0;
}

//////////////////////////////////////////////////////////////////////////
FontGlyphCache::~FontGlyphCache()
{
	foreach (GlyphImage* page, m_Pages)
	{
		SAFE_DELETE(page);
	}

	foreach (GlyphInfoMap::value_type pos, m_Glyphs)
	{
		SAFE_DELETE(pos.second);
	}
}

//////////////////////////////////////////////////////////////////////////
bool FontGlyphCache::HasGlyph(wchar_t ch)
{
	return (m_Glyphs.find(ch) != m_Glyphs.end());
}

//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::Initialize(size_t gridWidth, size_t gridHeight)
{
	m_GridWidth = gridWidth + 1;
	m_GridHeight = gridHeight + 1;

	m_TextureWidth = m_GridWidth * 20;

	const Ogre::RenderSystemCapabilities* caps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
	if (!caps->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES))
	{
		m_TextureWidth = Ogre::Bitwise::firstPO2From((Ogre::uint32)m_TextureWidth);
	}
	if (m_TextureWidth > 512) m_TextureWidth = 512;
}

//////////////////////////////////////////////////////////////////////////
GlyphInfo* FontGlyphCache::GetGlyph(wchar_t ch)
{
	GlyphInfoMap::const_iterator it;
	it = m_Glyphs.find(ch);
	if (it == m_Glyphs.end()) return NULL;

	return it->second;
}

//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::AddGlyph(wchar_t ch, int glyphIndex, FT_GlyphSlot glyphSlot, size_t width, size_t height, byte* pixels, size_t stride)
{
	if (stride == 0) stride = width;

	bool addPage = false;
	
	if (m_CurrentPage < 0) addPage = true;
	else
	{
		if (m_CurrentPosX + width >= m_TextureWidth)
		{
			m_CurrentPosX = 0;
			m_CurrentPosY += m_CurrentRowHeight;

			if (m_CurrentPosY + m_GridHeight >= (int)m_TextureWidth) addPage = true;
		}
	}

	if (addPage)
	{
		m_Pages.push_back(new GlyphImage(m_TextureWidth, m_TextureWidth));
		m_CurrentPage++;
		m_CurrentPosX = 0;
		m_CurrentPosY = 0;
		m_CurrentRowHeight = 0;
	}


	if (m_CurrentPosX + width >= m_TextureWidth || m_CurrentPosY + m_GridHeight >= m_TextureWidth)
	{
		Game::GetInstance()->Log(L"Error adding font glyph to cache (not enough space).");
		return;
	}


	if (pixels != NULL)	m_Pages[m_CurrentPage]->InsertGlyph(m_CurrentPosX, m_CurrentPosY, width, height, pixels, stride);

	m_Glyphs[ch] = new GlyphInfo(glyphIndex);

	if (pixels != NULL)
	{
		m_Glyphs[ch]->SetTexturePos(m_CurrentPosX, m_CurrentPosY, m_CurrentPosX + width, m_CurrentPosY + height, m_TextureWidth, m_CurrentPage);
		m_CurrentPosX += width + 1;
	}
	m_Glyphs[ch]->SetGlyphInfo(glyphSlot->advance.x / 64.f, glyphSlot->advance.y / 64.f, glyphSlot->bitmap_left, glyphSlot->bitmap_top);

	

	m_CurrentRowHeight = std::max(m_CurrentRowHeight, (int)height + 1);
}

//////////////////////////////////////////////////////////////////////////
Ogre::MaterialPtr& FontGlyphCache::GetMaterial(int pageIndex)
{
	return m_Pages[pageIndex]->GetMaterial();
}

//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::RebuildTextures()
{
	foreach (GlyphImage* page, m_Pages)
	{
		page->RebuildTexture();
	}
}

//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::DebugSave()
{
	int counter = 0;
	foreach (GlyphImage* page, m_Pages)
	{		
		Ogre::String fileName;
		fileName = "c:\\font_glyphs" + Ogre::StringConverter::toString(counter) + ".png";

		page->SaveToFile(fileName);
		counter++;
	}
}

//////////////////////////////////////////////////////////////////////////
// GlyphImage
//////////////////////////////////////////////////////////////////////////
FontGlyphCache::GlyphImage::GlyphImage(size_t width, size_t height)
{
	m_PixelData = new dword[width * height];
	memset(m_PixelData, 0, width * height * sizeof(dword));

	m_PixelBox = new Ogre::PixelBox(width, height, 1, Ogre::PF_A8R8G8B8, m_PixelData);
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
FontGlyphCache::GlyphImage::~GlyphImage()
{
	if (!m_Material.isNull())
	{
		Ogre::MaterialManager::getSingleton().remove(m_Material->getHandle());
		m_Material.setNull();
	}

	if (!m_Texture.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_Texture->getHandle());
		m_Texture.setNull();
	}

	SAFE_DELETE(m_PixelBox);
	SAFE_DELETE_ARRAY(m_PixelData);
}

//////////////////////////////////////////////////////////////////////////
Ogre::TexturePtr FontGlyphCache::GlyphImage::GetTexture()
{
	if (m_IsDirty) RebuildTexture();
	return m_Texture;
}

//////////////////////////////////////////////////////////////////////////
Ogre::MaterialPtr& FontGlyphCache::GlyphImage::GetMaterial()
{
	return m_Material;
}

//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::GlyphImage::RebuildTexture()
{
	if (!m_IsDirty) return;

	if (m_Texture.isNull())
	{
		m_Texture = Ogre::TextureManager::getSingleton().createManual(
			"__CachedFontGlyphTexture" + Ogre::StringConverter::toString(s_TextureCounter++),
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			m_PixelBox->getWidth(),
			m_PixelBox->getHeight(),
			0,
			Ogre::PF_A8R8G8B8);
	}

	Ogre::HardwarePixelBufferSharedPtr buf = m_Texture->getBuffer();
	buf->blitFromMemory(*m_PixelBox);


	// create material for the texture
	if (m_Material.isNull())
	{
		m_Material = Ogre::MaterialManager::getSingleton().create(m_Texture->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::TextureUnitState* texLayer = m_Material->getTechnique(0)->getPass(0)->createTextureUnitState(m_Texture->getName());

		// clamp to avoid fuzzy edges
		texLayer->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		// allow min/mag filter, but no mip
		texLayer->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);

		// set up blending
		if (m_Texture->hasAlpha())
		{
			m_Material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		}
		else
		{
			// use add if no alpha (assume black background)
			m_Material->setSceneBlending(Ogre::SBT_ADD);
		}

		m_Material->setCullingMode(Ogre::CULL_NONE);
		m_Material->setFog(Ogre::FOG_NONE);
		m_Material->setLightingEnabled(false);
		m_Material->setDepthCheckEnabled(false);
		m_Material->setDepthWriteEnabled(false);
	}
	
	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::GlyphImage::InsertGlyph(size_t posX, size_t posY, size_t width, size_t height, byte* pixels, size_t stride)
{
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			byte pixel = pixels[j * stride + i];
			byte* dest = static_cast<byte*>(m_PixelBox->data) + (((posY + j) * m_PixelBox->rowPitch + posX + i) * sizeof(dword));
			Ogre::PixelUtil::packColour(255u, 255u, 255u, pixel, m_PixelBox->format, static_cast<void*>(dest));
		}
	}

	m_IsDirty = true;
}


//////////////////////////////////////////////////////////////////////////
void FontGlyphCache::GlyphImage::SaveToFile(Ogre::String fileName)
{
	Ogre::Image img;
	img.loadDynamicImage(static_cast<Ogre::uchar*>(m_PixelBox->data), m_PixelBox->getWidth(), m_PixelBox->getHeight(), 1, Ogre::PF_A8R8G8B8, false);
	img.save(fileName);
}


} // namespace Wme
