// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "LineElement.h"
#include "Viewport.h"
#include "Region.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
LineElement::LineElement()
{
	m_Color = Ogre::ColourValue::White;	
	m_IsDirty = true;
	m_UpdateId = 0;

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

	mUseIdentityProjection = true;
	mUseIdentityView = true;
	mPolygonModeOverrideable = false;

	m_HWBufferInitialized = false;
	m_MaxLines = 0;
}
//////////////////////////////////////////////////////////////////////////
LineElement::~LineElement()
{
	SAFE_DELETE(m_RenderOperation.vertexData);
	if (!m_HardwareBuffer.isNull()) m_HardwareBuffer.setNull();
}

//////////////////////////////////////////////////////////////////////////
void LineElement::AddLine(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint)
{
	m_Lines.push_back(Line(startPoint, endPoint));
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void LineElement::AddRegion(const Region& region)
{
	const Region::PointVector& points = region.GetPoints();
	if (points.size() < 2) return;

	for (size_t i = 0; i < points.size(); i++)
	{
		size_t i2 = i + 1;
		if (i2 == points.size()) i2 = 0;

		AddLine(points[i], points[i2]);
	}
}

//////////////////////////////////////////////////////////////////////////
void LineElement::AddRectangle(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint)
{
	Ogre::Real x1 = startPoint.x;
	Ogre::Real y1 = startPoint.y;
	Ogre::Real x2 = endPoint.x;
	Ogre::Real y2 = endPoint.y;

	AddLine(Ogre::Vector2(x1, y1), Ogre::Vector2(x2, y1));
	AddLine(Ogre::Vector2(x2, y1), Ogre::Vector2(x2, y2));
	AddLine(Ogre::Vector2(x2, y2), Ogre::Vector2(x1, y2));
	AddLine(Ogre::Vector2(x1, y2), Ogre::Vector2(x1, y1));
}

//////////////////////////////////////////////////////////////////////////
void LineElement::ClearLines()
{
	m_Lines.clear();
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void LineElement::UpdateGeometry(Viewport* viewport, bool absolutePos)
{
	if (!m_IsDirty && !viewport->IsDirty(m_UpdateId)) return;

	m_UpdateId = viewport->GetUpdateId();

	if (!m_HWBufferInitialized)
	{
		m_RenderOperation.useIndexes = false;
		m_RenderOperation.vertexData = new Ogre::VertexData();
		Ogre::VertexDeclaration* decl = m_RenderOperation.vertexData->vertexDeclaration;

		size_t offset = 0;

		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

		m_HWBufferInitialized = true;
	}

	int numLines = m_Lines.size();
	if (numLines == 0) numLines = 1;

	if (numLines > m_MaxLines)
	{
		m_HardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			m_RenderOperation.vertexData->vertexDeclaration->getVertexSize(0),
			numLines * 2,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


		m_RenderOperation.vertexData->vertexBufferBinding->setBinding(0, m_HardwareBuffer);
		m_RenderOperation.operationType = Ogre::RenderOperation::OT_LINE_LIST;

		m_MaxLines = numLines;
	}

	FillLines(viewport, absolutePos);
	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void LineElement::FillLines(Viewport* viewport, bool absolutePos)
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	typedef struct
	{
		Ogre::Vector3 pos;
		Ogre::RGBA color;
	} LineVertex;


	SpriteVertex* points = new SpriteVertex[m_Lines.size() * 2];

	int index = 0;
	foreach (Line line, m_Lines)
	{
		points[index].pos.x = line.m_StartPoint.x;
		points[index].pos.y = line.m_StartPoint.y;
		points[index].pos.z = -1.0f;
		index++;

		points[index].pos.x = line.m_EndPoint.x;
		points[index].pos.y = line.m_EndPoint.y;
		points[index].pos.z = -1.0f;
		index++;
	}

	Ogre::uint32 color;
	rs->convertColourValue(m_Color, &color); // convert to platform native format

	for (size_t i = 0; i < m_Lines.size() * 2; i++)
	{
		points[i].color = color;
	}

	viewport->TransformVertices(points, m_Lines.size() * 2, absolutePos);


	// copy points to HW buffer
	LineVertex* verts = reinterpret_cast<LineVertex*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	for (size_t i = 0; i < m_Lines.size() * 2; i++)
	{
		verts[i].pos = points[i].pos;
		verts[i].color = points[i].color;
	}

	m_HardwareBuffer->unlock();

	delete [] points;
}

//////////////////////////////////////////////////////////////////////////
// IRenderableProvider
//////////////////////////////////////////////////////////////////////////
void LineElement::GetRenderables(IRenderableProvider::RenderableList& renderableList)
{
	renderableList.push_back(this);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Renderable implementation
//////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr& LineElement::getMaterial(void) const
{
	return m_Material;
}

//////////////////////////////////////////////////////////////////////////
void LineElement::getRenderOperation(Ogre::RenderOperation& op)
{
	op = m_RenderOperation;
	op.vertexData->vertexStart = 0;
	op.vertexData->vertexCount = m_Lines.size() * 2;
}

//////////////////////////////////////////////////////////////////////////
void LineElement::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::LightList& LineElement::getLights(void) const
{
	// N/A
	static Ogre::LightList ll;
	return ll;
}


} // namespace Wme
