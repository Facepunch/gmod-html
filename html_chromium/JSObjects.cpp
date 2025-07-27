#include "JSObjects.h"

JSArray::JSArray( const std::vector<JSValue>& data )
	: _data( data )
{}

JSArray::JSArray( std::vector<JSValue>&& data )
	: _data( std::move( data ) )
{}

const JSValue* JSArray::GetData( size_t& outSize ) const
{
	outSize = _data.size();
	return _data.data();
}

const std::vector<JSValue>& JSArray::GetInternalData() const
{
	return _data;
}

JSValue::IArray* JSArray::Copy() const
{
	return new JSArray( _data );
}

///

JSString::JSString( const std::string& data )
	: _data( data )
{}

JSString::JSString( std::string&& data )
	: _data( std::move( data ) )
{}

const char* JSString::GetData( size_t& outSize ) const
{
	outSize = _data.size();
	return _data.c_str();
}

JSValue::IString* JSString::Copy() const
{
	return new JSString( _data );
}

const std::string& JSString::GetInternalData() const
{
	return _data;
}

///

JSHashMap::JSHashMap( const std::unordered_map<std::string, JSValue>& data )
	: _data( data )
{}

JSHashMap::JSHashMap( std::unordered_map<std::string, JSValue>&& data )
	: _data( std::move( data ) )
{}

bool JSHashMap::Begin( const char*& pKey, size_t& keySize, const JSValue*& pValue )
{
	_iterator = _data.cbegin();
	return Next( pKey, keySize, pValue );
}

bool JSHashMap::Next( const char*& pKey, size_t& keySize, const JSValue*& pValue )
{
	if ( _iterator == _data.end() )
		return false;

	auto& pair = *_iterator;
	pKey = pair.first.c_str();
	keySize = pair.first.size();
	pValue = &pair.second;
	_iterator++;
	return true;
}

size_t JSHashMap::Size() const
{
	return _data.size();
}

JSValue::IHashMap* JSHashMap::Copy() const
{
	return new JSHashMap( _data );
}

const std::unordered_map<std::string, JSValue>& JSHashMap::GetInternalData() const
{
	return _data;
}
