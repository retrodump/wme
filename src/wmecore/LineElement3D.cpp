// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "LineElement3D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
LineElement3D::LineElement3D()
{
	m_IsDirty = true;
	m_MaxLines = 0;
	m_HWBufferInitialized = false;
	m_DepthCheck = true;
}

//////////////////////////////////////////////////////////////////////////
LineElement3D::~LineElement3D()
{
	SAFE_DELETE(mRenderOp.vertexData);
}

//////////////////////////////////////////////////////////////////////////
void LineElement3D::AddLine(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, const Ogre::ColourValue& color)
{
	m_Lines.push_back(Line3D(startPoint, endPoint, color));
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void LineElement3D::ClearLines()
{
	m_Lines.clear();
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void LineElement3D::AddBox(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::ColourValue& color)
{
	m_IsDirty = true;

	Ogre::Real maxx = max.x;
	Ogre::Real maxy = max.y;
	Ogre::Real maxz = max.z;

	Ogre::Real minx = min.x;
	Ogre::Real miny = min.y;
	Ogre::Real minz = min.z;

	AddLine(Ogre::Vector3(minx, miny, minz), Ogre::Vector3(maxx, miny, minz), color);
	AddLine(Ogre::Vector3(minx, miny, minz), Ogre::Vector3(minx, miny, maxz), color);
	AddLine(Ogre::Vector3(minx, miny, minz), Ogre::Vector3(minx, maxy, minz), color);
	AddLine(Ogre::Vector3(minx, maxy, minz), Ogre::Vector3(minx, maxy, maxz), color);
	AddLine(Ogre::Vector3(minx, maxy, minz), Ogre::Vector3(maxx, maxy, minz), color);
	AddLine(Ogre::Vector3(maxx, miny, minz), Ogre::Vector3(maxx, miny, maxz), color);
	AddLine(Ogre::Vector3(maxx, miny, minz), Ogre::Vector3(maxx, maxy, minz), color);
	AddLine(Ogre::Vector3(minx, maxy, maxz), Ogre::Vector3(maxx, maxy, maxz), color);
	AddLine(Ogre::Vector3(minx, maxy, maxz), Ogre::Vector3(minx, miny, maxz), color);
	AddLine(Ogre::Vector3(maxx, maxy, minz), Ogre::Vector3(maxx, maxy, maxz), color);
	AddLine(Ogre::Vector3(maxx, miny, maxz), Ogre::Vector3(maxx, maxy, maxz), color);
	AddLine(Ogre::Vector3(minx, miny, maxz), Ogre::Vector3(maxx, miny, maxz), color);
}

//////////////////////////////////////////////////////////////////////////
void LineElement3D::UpdateGeometry()
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

	int numLines = m_Lines.size();
	if (numLines == 0) numLines = 1;

	// increase HW buffer if necessary
	if (numLines > m_MaxLines)
	{
		Ogre::HardwareVertexBufferSharedPtr vBuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
			numLines * 2,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


		mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vBuf);		
		mRenderOp.vertexData->vertexStart = 0;
		mRenderOp.operationType = Ogre::RenderOperation::OT_LINE_LIST;

		m_MaxLines = numLines;
	}
	mRenderOp.vertexData->vertexCount = numLines * 2;

	
	
	typedef struct
	{
		Ogre::Vector3 pos;
		Ogre::RGBA color;
	} LineVertex;

		
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	

	// fill vertices and update the bounding box
	Ogre::HardwareVertexBufferSharedPtr vBuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	LineVertex* verts = reinterpret_cast<LineVertex*>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));


	mBox.setNull();

	int index = 0;
	foreach (Line3D line, m_Lines)
	{
		// convert to platform native format
		Ogre::uint32 color;
		rs->convertColourValue(line.m_Color, &color);

		verts[index].pos = line.m_StartPoint;
		verts[index].color = color;
		index++;

		verts[index].pos = line.m_EndPoint;
		verts[index].color = color;
		index++;

		mBox.merge(line.m_StartPoint);
		mBox.merge(line.m_EndPoint);
	}

	vBuf->unlock();

	SetMaterial(m_DepthCheck);
	
	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void LineElement3D::SetMaterial(bool depthCheck)
{
	Utf8String matName;

	if (depthCheck) matName = "sys/line3D_material";
	else matName = "sys/line3D_material_nodepth";

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
Ogre::Real LineElement3D::getSquaredViewDepth(const Ogre::Camera* cam) const
{
	Ogre::Vector3 min, max, mid, dist;
	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((max - min) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;
	
	return dist.squaredLength();
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real LineElement3D::getBoundingRadius() const
{
	return 0.0f;
}

//////////////////////////////////////////////////////////////////////////
void LineElement3D::getWorldTransforms(Ogre::Matrix4* xform) const
{
	Ogre::SimpleRenderable::getWorldTransforms(xform);
}


} // namespace Wme
