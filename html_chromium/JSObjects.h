#include "html/JSValue.h"
#include <vector>
#include <string>
#include <unordered_map>

// I'm okay with the events that are unfolding currently.

class JSArray : public JSValue::IArray
{
private:
	JSArray( const std::vector<JSValue>& data );
	JSArray( std::vector<JSValue>&& data );

public:
	~JSArray() override = default;

	const JSValue* GetData( size_t& outSize ) const override;
	IArray* Copy() const override;

	const std::vector<JSValue>& GetInternalData() const;

	template<typename T>
	static JSValue Create( T&& data )
	{
		return { new JSArray( std::forward<T>( data ) ) };
	}

private:
	const std::vector<JSValue> _data;
};

///

class JSString : public JSValue::IString
{
private:
	JSString( const std::string& data );
	JSString( std::string&& data );

public:
	~JSString() override = default;

	const char* GetData( size_t& outSize ) const override;
	IString* Copy() const override;

	const std::string& GetInternalData() const;

	template<typename T>
	static JSValue Create( T&& data )
	{
		return { new JSString( std::forward<T>( data ) ) };
	}

private:
	const std::string _data;
};

///

class JSHashMap : public JSValue::IHashMap
{
private:
	JSHashMap( const std::unordered_map<std::string, JSValue>& data );
	JSHashMap( std::unordered_map<std::string, JSValue>&& data );

public:
	bool Begin( const char*& pKey, size_t& keySize, const JSValue*& pValue ) override;
	bool Next( const char*& pKey, size_t& keySize, const JSValue*& pValue ) override;

	size_t Size() const override;

	IHashMap* Copy() const override;

	const std::unordered_map<std::string, JSValue>& GetInternalData() const;

	template<typename T>
	static JSValue Create( T&& data )
	{
		return { new JSHashMap( std::forward<T>( data ) ) };
	}

private:
	std::unordered_map<std::string, JSValue> _data;
	std::unordered_map<std::string, JSValue>::const_iterator _iterator;
};