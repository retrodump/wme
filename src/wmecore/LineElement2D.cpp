// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "LineElement2D.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
LineElement2D::LineElement2D()
{
	// retreive / create material for line drawing
	Ogre::MaterialManager::ResourceCreateOrRetrieveResult res = Ogre::MaterialManager::getSingleton().createOrRetrieve("sys/line2D_material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_Material = res.first;
	if (res.second)
	{
		m_Material->setReceiveShadows(false);
		m_Material->setCullingMode(Ogre::CULL_NONE);
		m_Material->setFog(Ogre::FOG_NONE);
		m_Material->setLightingEnabled(false);
		m_Material->setDepthCheckEnabled(false);
		m_Material->setDepthWriteEnabled(false);
		m_Material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	}
}

//////////////////////////////////////////////////////////////////////////
LineElement2D::~LineElement2D()
{
}

//////////////////////////////////////////////////////////////////////////
void LineElement2D::AddGeometry()
{
	size_t numVerts = 8;
	Vertex2D* verts = new Vertex2D[numVerts];

	verts[0].pos = Ogre::Vector2(-25, -50);
	verts[1].pos = Ogre::Vector2(25, -50);
	
	verts[2].pos = Ogre::Vector2(25, -50);
	verts[3].pos = Ogre::Vector2(25, 50);

	verts[4].pos = Ogre::Vector2(25, 50);
	verts[5].pos = Ogre::Vector2(-25, 50);

	verts[6].pos = Ogre::Vector2(-25, 50);
	verts[7].pos = Ogre::Vector2(-25, -50);

	for (size_t i = 0; i < numVerts; i++)
	{
		verts[i].color = Ogre::ColourValue::Red;
	}

	m_ParentNode->AddGeometry(verts, numVerts, m_Material, Ogre::RenderOperation::OT_LINE_LIST);

	delete [] verts;
}


} // namespace Wme
