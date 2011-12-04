// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScript_H__
#define __WmeScript_H__

#include "Object.h"
#include <boost/smart_ptr.hpp>
#include <list>
#include <deque>
#include <stack>
#include "Game.h"
#include "Uuid.h"
#include "Value.h"
#include "Stack.h"
#include "ValueManager.h"

namespace Wme
{
	class ScriptManager;
	class Script;
	class ScriptableObject;
	class DynamicBuffer;

	typedef boost::shared_ptr<Script> ScriptPtr;
	typedef std::list<ScriptPtr> Scripts;


	class WmeDllExport Script : public Object
	{
	public:
		Script(ScriptManager* mgr, ScriptableObject* owner);
		Script(Script* parentScript, ScriptableObject* owner, Script* requestingScript, const WideString& methodName, bool async);
		virtual ~Script();

		ScriptableObject* GetOwnerObject() const
		{
			return m_OwnerObject;
		};

		ScriptManager* GetOwnerMgr() const
		{
			return m_OwnerMgr;
		};

		enum ScriptState
		{
			SCRIPT_RUNNING,
			SCRIPT_IDLE,
			SCRIPT_SLEEPING,
			SCRIPT_WAITING_FOR_OBJECT,
			SCRIPT_WAITING_FOR_SCRIPT,
			SCRIPT_FINISHED,
			SCRIPT_FINISHED_THREAD,
			SCRIPT_ERROR
		};

		void SetState(ScriptState state);
		ScriptState GetState() const
		{
			return m_State;
		};

		bool IsRunning() const;
		bool IsFinished() const;

		void DecoupleOwnerAndFinish();
		void StopChildThreads();
		
		Script* CreateChildThread(Script* requestingScript, const WideString& methodName, ScriptableObject* owner, bool async);
		void RemoveChildThread(ScriptPtr childThread);

		void Tick();

		bool LoadFromBuffer(DynamicBuffer* buffer);

		Uuid GetUuid() const { return m_Uuid; }

		Stack* GetStack() { return &m_Stack; }

		Script* GetParentScript() {	return m_ParentScript; }

		void RuntimeError(const WideString& str);

		WideString GetStringFromTable(int stringID) const;
		WideString GetFileNameFromTable(int fileID) const;

		bool HasMethod(const WideString& name) const;

		void DumpToFile(const WideString& fileName) const;

		bool WaitForObject(ScriptableObject* object);
		bool WaitForScript(ScriptPtr script);
		bool SleepFor(unsigned long time);

		void ResolveWaitingStates();

		bool IsChildThread() const { return m_IsChildThread; }

		DynamicBuffer* GetBytecode() { return m_Bytecode; }

		void AddRootsToGrayList(ValueManager* valMgr);

		WideString GetFileName() const { return m_FileName; }
		void SetFileName(const WideString& fileName) { m_FileName = fileName; }

	private:
		bool m_IsChildThread;
		bool m_IsAsyncChildThread;
		Scripts m_ChildThreads;
		ScriptManager* m_OwnerMgr;
		ScriptableObject* m_OwnerObject;
		Script* m_ParentScript;
		ScriptState m_State;

		WideString m_FileName;

		Uuid m_Uuid;
		DynamicBuffer* m_Bytecode;

		typedef std::vector<WideString> FilenameVector;
		FilenameVector m_FileTable;

		typedef std::vector<WideString> WideStringVector;
		WideStringVector m_StringTable;

		typedef std::map<WideString, int> EntryPointMap;
		EntryPointMap m_FunctionTable;
		EntryPointMap m_MethodTable;

		int GetEntryPointAddress(const WideString& name) const;		

		int m_CurrentLine;
		int m_CurrentFile;
		Stack m_Stack;
		std::stack<int> m_CallStack;

		Value* GetVariableRef(const WideString& name);
		Value* GetScriptGlobalVariableRef(const WideString& name);
		void AddLocalVariable(const WideString& name);
		void AddGlobalVariable(const WideString& name);


		ScriptPtr m_WaitForScript;
		int m_WaitForObjectHandle;
		unsigned long m_SleepEndTime;

		unsigned long GetCurrentTime();


		class WmeDllExport ScriptScope
		{
		public:
			enum ContextType
			{
				CONTEXT_GLOBAL,
				CONTEXT_FUNCTION,
				CONTEXT_METHOD
			};

			ScriptScope(const WideString& context, ContextType contextType)
			{
				m_Context = context;
				m_ContextType = contextType;
			};

			Value* GetVariableRef(const WideString& name)
			{
				if (m_Vars.HasProperty(name)) return m_Vars.GetProperty(name);
				else return NULL;
			};

			void AddVariable(const WideString& name)
			{
				if (!m_Vars.HasProperty(name))
				{
					m_Vars.SetProperty(name, NULL);
				}
			}

			WideString GetContext() const
			{
				return m_Context;
			};

			ContextType GetContextType() const
			{
				return m_ContextType;
			};

			bool IsSameContext(const WideString& context, ContextType contextType) const
			{
				return (m_Context == context && m_ContextType == contextType);
			};

			void AddRootsToGrayList(ValueManager* valMgr)
			{
				m_Vars.AddPropertiesToGrayList(valMgr);
			}
		private:
			Value m_Vars;
			WideString m_Context;
			ContextType m_ContextType;

		};
		typedef std::deque<ScriptScope> ScopeStack;
		ScopeStack m_ScopeStack;

	};
	
}

#endif // __WmeScript_H__