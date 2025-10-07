#pragma once

#include <stddef.h>

// Tagged union representing a JavaScript value. We can throw them in vectors and shit without any issues.
class JSValue
{
public:
	class IArray
	{
	public:
		virtual ~IArray() {}
		virtual const JSValue* GetData( size_t& outSize ) const = 0;
		virtual IArray* Copy() const = 0;
	};

	class IString
	{
	public:
		virtual ~IString() {}
		virtual const char* GetData( size_t& outSize ) const = 0;
		virtual IString* Copy() const = 0;
	};

	class IHashMap
	{
	public:
		virtual ~IHashMap() {}

		// The implementation will hold iterator state internally, so don't fuck up
		virtual bool Begin( const char*& pKey, size_t& keySize, const JSValue*& pValue ) = 0;
		virtual bool Next( const char*& pKey, size_t& keySize, const JSValue*& pValue ) = 0;

		virtual size_t Size() const = 0;

		virtual IHashMap* Copy() const = 0;
	};

	///

	enum class Type
	{
		Undefined,
		Bool,
		Number,
		Array,
		String,
		HashMap
	};

	JSValue()
		: _type( Type::Undefined )
	{}

	JSValue( bool value )
		: _type( Type::Bool )
		, _bool( value )
	{}

	JSValue( double value )
		: _type( Type::Number )
		, _number( value )
	{}

	JSValue( IString* pString )
		: _type( Type::String )
		, _pString( pString )
	{}

	JSValue( IArray* pArray )
		: _type( Type::Array )
		, _pArray( pArray )
	{}

	JSValue( IHashMap* pHashMap )
		: _type( Type::HashMap )
		, _pHashMap( pHashMap )
	{}

	JSValue( const JSValue& other )
		: _type( other._type )
	{
		switch ( _type )
		{
			case Type::Undefined:
				break;
			case Type::Bool:
				_bool = other._bool;
				break;
			case Type::Number:
				_number = other._number;
				break;
			case Type::String:
				_pString = other._pString->Copy();
				break;
			case Type::Array:
				_pArray = other._pArray->Copy();
				break;
			case Type::HashMap:
				_pHashMap = other._pHashMap->Copy();
				break;
		}
	}

	JSValue( JSValue&& other ) noexcept
		: _type( other._type )
	{
		switch ( _type )
		{
			case Type::Undefined:
				break;
			case Type::Bool:
				_bool = other._bool;
				break;
			case Type::Number:
				_number = other._number;
				break;
			case Type::String:
				_pString = other._pString;
				break;
			case Type::Array:
				_pArray = other._pArray;
				break;
			case Type::HashMap:
				_pHashMap = other._pHashMap;
				break;
		}

		other._type = Type::Undefined;
	}

	JSValue& operator=( const JSValue& other )
	{
		if ( &other == this )
			return *this;

		Invalidate();
		_type = other._type;

		switch ( _type )
		{
			case Type::Undefined:
				break;
			case Type::Bool:
				_bool = other._bool;
				break;
			case Type::Number:
				_number = other._number;
				break;
			case Type::String:
				_pString = other._pString->Copy();
				break;
			case Type::Array:
				_pArray = other._pArray->Copy();
				break;
			case Type::HashMap:
				_pHashMap = other._pHashMap->Copy();
				break;
		}

		return *this;
	}

	JSValue& operator=( JSValue&& other ) noexcept
	{
		Invalidate();
		_type = other._type;

		switch ( _type )
		{
			case Type::Undefined:
				break;
			case Type::Bool:
				_bool = other._bool;
				break;
			case Type::Number:
				_number = other._number;
				break;
			case Type::String:
				_pString = other._pString;
				break;
			case Type::Array:
				_pArray = other._pArray;
				break;
			case Type::HashMap:
				_pHashMap = other._pHashMap;
				break;
		}

		other._type = Type::Undefined;
		return *this;
	}

	~JSValue()
	{
		Invalidate();
	}

	Type GetType() const
	{
		return _type;
	}

	bool GetBool() const
	{
		return _bool;
	}

	double GetNumber() const
	{
		return _number;
	}

	//

	const char* GetString( size_t& outSize ) const
	{
		return _pString->GetData( outSize );
	}

	const IString* GetInternalString() const
	{
		return _pString;
	}

	//

	const JSValue* GetArray( size_t& outSize ) const
	{
		return _pArray->GetData( outSize );
	}

	const IArray* GetInternalArray() const
	{
		return _pArray;
	}

	//

	const bool HashMap_Begin( const char*& pKey, size_t& keySize, const JSValue*& pValue ) const
	{
		return _pHashMap->Begin( pKey, keySize, pValue );
	}

	const bool HashMap_Next( const char*& pKey, size_t& keySize, const JSValue*& pValue ) const
	{
		return _pHashMap->Next( pKey, keySize, pValue );
	}

	const size_t HashMap_Size() const
	{
		return _pHashMap->Size();
	}

	const IHashMap* GetInternalHashMap() const
	{
		return _pHashMap;
	}

private:
	void Invalidate() noexcept
	{
		switch ( _type )
		{
			case Type::Undefined:
			case Type::Bool:
			case Type::Number:
				break;
			case Type::String:
				delete _pString;
				break;
			case Type::Array:
				delete _pArray;
				break;
			case Type::HashMap:
				delete _pHashMap;
				break;
		}

		_type = Type::Undefined;
	}

	Type _type;

	union
	{
		bool      _bool;        // Type::Bool
		double    _number;      // Type::Number
		IArray* _pArray;      // Type::Array
		IString* _pString;     // Type::String
		IHashMap* _pHashMap;    // Type::HashMap
	};
};
