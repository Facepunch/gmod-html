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
		, m_Size( 0 )
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

	bool ResizeData(int wide, int tall)
	{
		bool resized = wide * tall * 4 > m_Size;
		if ( resized )
		{
			unsigned char* old_Data = m_Data;
			m_Size = wide * tall * 4;
			m_Data = new unsigned char[m_Size];
			delete[] old_Data;
		}

		m_Wide = wide;
		m_Tall = tall;
		return resized;
	}

	void Blit(const unsigned char* data, int x, int y, int width, int height)
	{
		if (x + width > m_Wide || y + height > m_Tall) {
			// TODO: Add error log here for invalid Blit.
			return;
		}

		for ( int SrcY = 0; SrcY < height; SrcY++ )
		{
			// TODO: See if AVX copy or SIMD can speed this up?
			memcpy( &m_Data[( SrcY + y ) * m_Wide * 4 + x * 4], &data[( SrcY + y ) * m_Wide * 4 + x * 4], width * 4 );
		}
		m_Dirty = true;
	}

	void BlitRelative(const unsigned char* data, int x, int y, int width, int height)
	{
		if (x + width > m_Wide || y + height > m_Tall) {
			// TODO: Add error log here for invalid Blit.
			return;
		}
		
		for ( int SrcY = 0; SrcY < height; SrcY++ )
		{
			memcpy( &m_Data[( SrcY + y ) * m_Wide * 4 + x * 4], &data[SrcY * width * 4], width * 4 );
		}
		m_Dirty = true;
	}

	void SetData( const unsigned char* data, int wide, int tall )
	{
		if (wide > m_Wide || tall > m_Tall) {
			// TODO: Add warning log here for invalid data set. Call ResizeData first.
			ResizeData(wide, tall);
		}

		memcpy( m_Data, data, wide * tall * 4 );
		m_Dirty = true;
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

private:
	bool m_Dirty;

	int m_Wide;
	int m_Tall;
	int m_Size;

	base::Lock m_Lock;
	unsigned char* m_Data;
};
