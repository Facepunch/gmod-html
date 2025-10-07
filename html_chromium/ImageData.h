#pragma once

#include "cef_start.h"
#include "include/cef_base.h"
#include "include/base/cef_lock.h"
#include "cef_end.h"

class ImageData
{
public:
	ImageData()
		: m_Dirty( false )
		, m_Wide( 0 )
		, m_Tall( 0 )
		, m_Data( nullptr )
	{}

	~ImageData()
	{
		delete[] m_Data;
	}

	void Lock()
	{
		m_Lock.Acquire();
	}

	void Unlock()
	{
		m_Lock.Release();
	}

	void SetData( const unsigned char* data, int wide, int tall )
	{
		if ( m_Wide != wide || m_Tall != tall )
		{
			delete[] m_Data;
			m_Data = new unsigned char[wide * tall * 4];
		}

		memcpy( m_Data, data, wide * tall * 4 );
		m_Wide = wide;
		m_Tall = tall;
		m_Dirty = true;
	}

	unsigned char* GetData( int& wide, int& tall )
	{
		wide = m_Wide;
		tall = m_Tall;
		return m_Data;
	}

	bool IsDirty()
	{
		return m_Dirty;
	}

	void SetDirty( bool dirty )
	{
		m_Dirty = dirty;
	}

private:
	friend class ChromiumBrowser;
	bool m_Dirty;

	int m_Wide;
	int m_Tall;

	base::Lock m_Lock;
	unsigned char* m_Data;
};
