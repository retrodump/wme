// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeValue_H__
#define __WmeValue_H__

namespace Wme
{
	class ScriptableObject;
	class ValueManager;

	class WmeDllExport Value
	{
	public:
		enum ValueType
		{
			VAL_NULL,
			VAL_STRING,
			VAL_INT,
			VAL_DOUBLE,
			VAL_BOOL,
			VAL_OBJECT,
			VAL_REFERENCE,
			VAL_NATIVE
		};

		enum GCMarkType
		{
			MARK_WHITE,
			MARK_GRAY,
			MARK_BLACK
		};

		typedef std::list<Value*> ValueList;


		Value();
		virtual ~Value();

		// factory functions
		static Value* Create();
		static Value* Create(int val);
		static Value* Create(const WideString& val);
		static Value* Create(double val);
		static Value* Create(bool val);
		static Value* Create(ScriptableObject* val);
		

		void Clear();
		void ClearValue();

		// arithmetic operations
		void Add(const Value* val);
		void Sub(const Value* val);
		void Mul(const Value* val);
		void Div(const Value* val);
		void Modulo(const Value* val);
		void Neg();
		void Increment();
		void Decrement();

		ValueType GetType() const;
		ValueType GetExactType() const;

		void SetNull();
		void SetValue(int val);
		void SetValue(const WideString& val);
		void SetValue(double val);
		void SetValue(bool val);
		void SetValue(ScriptableObject* val);
		void SetReference(Value* val, const WideString& prop);

		void SetValue(Value* val);
		Value* GetValue();

		bool IsNull() const;
		int GetInt(int defaultVal = 0) const;
		WideString GetString(const WideString& defaultVal = L"(null)") const;
		double GetDouble(double defaultVal = 0.0) const;
		bool GetBool(bool defaultVal = false) const;
		ScriptableObject* GetNative() const;
		Value* GetReference() const;
		WideString GetPropReference() const;


		void SetProperty(const WideString& name, Value* val);
		Value* GetProperty(const WideString& name) const;
		Value* GetPropertySafe(const WideString& name);
		bool HasProperty(const WideString& name) const;

		bool IsValueType() const
		{
			return GetExactType() != VAL_OBJECT && GetExactType() != VAL_REFERENCE;
		}

		typedef std::map<WideString, Value*> ValueCollection;

		const ValueCollection& GetProperties() const
		{
			return m_Properties;
		}

		int Compare(const Value* val) const;
		bool StrongEquals(const Value* val) const;
		WideString GetTypeName() const;

		void SetGCMark(GCMarkType type)
		{
			m_GCMark = type;
		}
		GCMarkType GetGCMark()
		{
			return m_GCMark;
		}

		void AddPropertiesToGrayList(ValueManager* valMgr) const;
		void AddReferencesToGrayList(ValueManager* valMgr) const;

#ifdef WME_GC_WRITE_BARRIER_CHECKING
		bool HasWhiteReferences() const;
#endif

	private:
		bool IsValidForArithmetics(const Value* val);

		ValueType m_Type;

		WideString m_ValString;
		int m_ValInt;
		double m_ValDouble;
		bool m_ValBool;
		ValueCollection m_Properties;
		WideString m_PropReference;
		Value* m_ValReference;
		int m_ValNative;

		GCMarkType m_GCMark;
		
	};
}

#endif
