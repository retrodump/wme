// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMicroTask_H__
#define __WmeMicroTask_H__


#include "Object.h"


namespace Wme
{
	class WmeDllExport MicroTask : public Object
	{
	public:
		MicroTask();
		virtual ~MicroTask();

		virtual void OnActivate() {};
		virtual void OnDeactivate() {};
		virtual void OnUpdate() {};

		bool IsActive() const { return m_IsActive; }
		void SetActive();

		bool IsFinished() const { return m_IsFinished; }
		void SetFinished();

	private:
		bool m_IsActive;
		bool m_IsFinished;
	};
}

#endif // __WmeMicroTask_H__
