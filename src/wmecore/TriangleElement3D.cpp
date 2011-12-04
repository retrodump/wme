// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TriangleElement3D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TriangleElement3D::TriangleElement3D()
{
	m_IsDirty = true;
	m_MaxTriangles = 0;
	m_HWBufferInitialized = false;
	m_DepthCheck = true;
}

//////////////////////////////////////////////////////////////////////////
TriangleElement3D::~TriangleElement3D()
{
	SAFE_DELETE(mRenderOp.vertexData);
}

//////////////////////////////////////////////////////////////////////////
void TriangleElement3D::AddTriangle(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC, const Ogre::ColourValue& color)
{
	m_Triangles.push_back(Triangle3D(pointA, pointB, pointC, color));
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void TriangleElement3D::ClearTriangles()
{
	m_Triangles.clear();
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void TriangleElement3D::UpdateGeometry()
{
	if (!m_IsDirty) return;


	if (!m_HWBufferInitialized)
	{
		mRenderOp.useIndexes = false;
		mRenderOp.vertexData = new Ogre::VertexData();
		Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;

		size_t offset = 0;

		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

		m_HWBufferInitialized = true;
	}

	int numTris = m_Triangles.size();
	if (numTris == 0) numTris = 1;

	// increase HW buffer if necessary
	if (numTris > m_MaxTriangles)
	{
		Ogre::HardwareVertexBufferSharedPtr vBuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
			numTris * 3,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


		mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vBuf);		
		mRenderOp.vertexData->vertexStart = 0;
		mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

		m_MaxTriangles = numTris;
	}
	mRenderOp.vertexData->vertexCount = numTris * 3;



	typedef struct
	{
		Ogre::Vector3 pos;
		Ogre::RGBA color;
	} TriVertex;


	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();


	// fill vertices and update the bounding box
	Ogre::HardwareVertexBufferSharedPtr vBuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	TriVertex* verts = reinterpret_cast<TriVertex*>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));


	mBox.setNull();

	int index = 0;
	foreach (Triangle3D tri, m_Triangles)
	{
		// convert to platform native format
		Ogre::uint32 color;
		rs->convertColourValue(tri.m_Color, &color);

		for (int i = 0; i < 3; i++)
		{
			verts[index].pos = tri.m_Points[i];
			verts[index].color = color;
			index++;

			mBox.merge(tri.m_Points[i]);
		}
	}

	vBuf->unlock();

	SetMaterial(m_DepthCheck);

	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void TriangleElement3D::SetMaterial(bool depthCheck)
{
	Utf8String matName;

	if (depthCheck) matName = "sys/triangle3D_material";
	else matName = "sys/triangle3D_material_nodepth";

	Ogre::MaterialManager::ResourceCreateOrRetrieveResult res = Ogre::MaterialManager::getSingleton().createOrRetrieve(matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::MaterialPtr mat = res.first;
	if (res.second)
	{
		mat->setReceiveShadows(false);
		mat->getTechnique(0)->setLightingEnabled(false);
		mat->setCullingMode(Ogre::CULL_NONE);
		mat->setFog(Ogre::FOG_NONE);
		mat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

		if (!depthCheck)
		{
			mat->setDepthCheckEnabled(false);
			mat->setDepthWriteEnabled(false);
		}
	}
	this->setCastShadows(false);
	this->setMaterial(matName);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::SimpleRenderable
//////////////////////////////////////////////////////////////////////////
Ogre::Real TriangleElement3D::getSquaredViewDepth(const Ogre::Camera* cam) const
{
	Ogre::Vector3 min, max, mid, dist;
	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((max - min) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;

	return dist.squaredLength();
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real TriangleElement3D::getBoundingRadius() const
{
	return 0.0f;
}

//////////////////////////////////////////////////////////////////////////
void TriangleElement3D::getWorldTransforms(Ogre::Matrix4* xform) const
{
	Ogre::SimpleRenderable::getWorldTransforms(xform);
}


} // namespace Wme