// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeValueManager_H__
#define __WmeValueManager_H__

#include "Object.h"
#include "Value.h"
#include <list>

namespace Wme
{

	class WmeDllExport ValueManager : public Object
	{
	public:
		enum GCState
		{
			GC_IDLE,
			GC_TRACKING,
			GC_DELETING
		};

		ValueManager();
		virtual ~ValueManager();

		/// Creates a new value tracked by garbage collector.
		Value* CreateValue();

		/// Invoke garbage collecting on existing values.
		void CollectGarbage();

		/// Adds a single value to a gray list.
		/// Only adds the value if it's unmarked and if the garbage collector
		/// is currently tracking
		void AddToGrayList(Value* val);

		GCState GetGCState()
		{
			return m_GCState;
		}

		void WriteBarrier(Value* parentVal, Value* newVal);

		WideString GetInfo() const;

	private:		
		Value::ValueList m_Values;

		/// Marks all values by given type.
		/// white = unprocessed
		/// gray = added to list, but not tracked yet
		/// black = processed
		void MarkAll(Value::GCMarkType type);

		/// Scans all known values and adds them to gray list
		/// as a starting point for value tracking.
		/// @see	TrackValues()
		void AddRootsToGrayList();

		/// Incrementally marks all referenced ("black") values.
		/// @return	Returns true if there's still work to be done
		bool TrackValues();

		/// Deletes all non-marked ("white") values.
		/// @return	Returns true if there's still work to be done
		bool DeleteValues();

#ifdef WME_GC_WRITE_BARRIER_CHECKING
		void WriteBarrierCheck();
#endif

		Value::ValueList m_GrayList;
		bool m_FirstRun;
		GCState m_GCState;
		
		// GC parameters
		size_t m_NumToTrackPerFrame;
		size_t m_NumToDeletePerFrame;
		size_t m_NumFramesToStartGC;
		size_t m_GrowthToStartGC;
		

		// GC stats
		size_t m_FramesSinceLastCollect;
		size_t m_GrowthSinceLastCollect;
		size_t m_PeakNumValues;

		// pool
		typedef std::list<Value*> ValueList;
		ValueList m_ValuePool;

		void InitPool();
		size_t m_MaxPoolSize;
		size_t m_InitialPoolSize;
	};
}

#endif // __WmeValueManager_H__
