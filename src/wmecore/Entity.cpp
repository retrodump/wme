// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Entity.h"
#include "MicroTask.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Entity::Entity()
{
	m_Active = true;
}

//////////////////////////////////////////////////////////////////////////
Entity::~Entity()
{
	ClearTasks();
}


//////////////////////////////////////////////////////////////////////////
void Entity::ClearTasks()
{
	foreach (MicroTask* task, m_MicroTasks)
	{
		SAFE_DELETE(task);
	}
	m_MicroTasks.clear();
}

//////////////////////////////////////////////////////////////////////////
void Entity::UpdateTasks()
{
	while (true)
	{
		MicroTask* currentTask = GetCurrentTask();
		if (!currentTask) break;

		// remove finished tasks
		if (currentTask->IsFinished())
		{
			SAFE_DELETE(currentTask);
			m_MicroTasks.pop_front();
			continue;
		}

		currentTask->SetActive();
		currentTask->OnUpdate();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
MicroTask* Entity::GetCurrentTask()
{
	if (m_MicroTasks.empty()) return NULL;
	else return m_MicroTasks.front();
}

//////////////////////////////////////////////////////////////////////////
void Entity::EnqueueTask(MicroTask* task)
{
	m_MicroTasks.push_back(task);
}

//////////////////////////////////////////////////////////////////////////
void Entity::SetTask(MicroTask* task)
{
	foreach (MicroTask* task, m_MicroTasks)
	{
		task->SetFinished();
	}
	EnqueueTask(task);
}


//////////////////////////////////////////////////////////////////////////
bool Entity::LoadFromXml(TiXmlElement* rootNode)
{
	return InteractiveObject::LoadFromXml(rootNode);
}

//////////////////////////////////////////////////////////////////////////
bool Entity::SaveToXml(TiXmlElement* rootNode)
{
	return InteractiveObject::SaveToXml(rootNode);
}


} // namespace Wme
