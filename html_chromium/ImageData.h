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
		size_t bufSize = static_cast<size_t>(wide) * static_cast<size_t>(tall) * 4;

		if ( m_Wide != wide || m_Tall != tall )
		{
			delete[] m_Data;
			m_Data = new unsigned char[bufSize];
		}

		// Allocation failed.
		if ( !m_Data )
		{
			m_Wide = 0;
			m_Tall = 0;
			m_Dirty = true;
			return;
		}

		memcpy( m_Data, data, bufSize );
		m_Wide = wide;
		m_Tall = tall;
		m_Dirty = true;
	}

	void CopyImage( const unsigned char* data, int x, int y, int w, int h )
	{
		if ( w < 0 || h < 0 || !data || !m_Data ) return;

		const int dstX = max( 0, x );
		const int dstY = max( 0, y );
		const int srcX = max( 0, -x );
		const int srcY = max( 0, -y );

		const int copyW = min( w - srcX, m_Wide - dstX );
		const int copyH = min( h - srcY, m_Tall - dstY );

		if ( copyW > 0 && copyH > 0 )
		{
			for ( int row = 0; row < copyH; ++row )
			{
				const size_t dstOffset = static_cast<size_t>( ( dstY + row ) * m_Wide + dstX ) * 4;
				const size_t srcOffset = static_cast<size_t>( ( srcY + row ) * w + srcX ) * 4;
				memcpy( &m_Data[dstOffset], &data[srcOffset], static_cast<size_t>( copyW ) * 4 );
			}

			m_Dirty = true;
		}
	}

	const unsigned char* GetData( int& wide, int& tall )
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

	ImageData( const ImageData& ) = delete;
	ImageData& operator=( const ImageData& ) = delete;

private:
	friend class ChromiumBrowser;
	bool m_Dirty;

	int m_Wide;
	int m_Tall;

	base::Lock m_Lock;
	unsigned char* m_Data;
};
