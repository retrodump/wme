// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEntity_H__
#define __WmeEntity_H__


#include "InteractiveObject.h"
#include "DocumentAwareObject.h"


namespace Wme
{
	class MicroTask;
	class Viewport;

	class WmeDllExport Entity : public InteractiveObject, public DocumentAwareObject
	{
	public:
		typedef Entity* (*EntityActivator)();

		Entity();
		virtual ~Entity();

		typedef std::list<Entity*> EntityList;

		virtual WideString GetEntitySubtype() const { return L"Entity"; }

		void ClearTasks();
		void UpdateTasks();
		MicroTask* GetCurrentTask();
		void EnqueueTask(MicroTask* task);
		void SetTask(MicroTask* task);

		virtual void Update() {}
		virtual void PreRender(Viewport* viewport) {}

		bool IsActive() const { return m_Active; }
		virtual void SetActive(bool val) { m_Active = val; }

		// internal
		virtual void OnPreFindVisibleObjects() {};
		virtual void OnPostFindVisibleObjects() {};

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);

	protected:
		bool m_Active;

	private:
		typedef std::list<MicroTask*> MicroTaskList;
		MicroTaskList m_MicroTasks;
	};
}


#endif // __WmeEntity_H__
