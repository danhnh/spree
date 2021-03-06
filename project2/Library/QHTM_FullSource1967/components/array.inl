/*----------------------------------------------------------------------
Copyright (c) 1998 Lee Alexander
Please see the file "licence.txt" for licencing details.
File:		Array.h
Owner:	leea
Purpose:Array implementation
----------------------------------------------------------------------*/
template<class T>
CArray<T>::CArray()
	:	m_pItems( NULL )
	,	m_uGrowBy( 1 )
	,	m_uItemsAllocated( 0 )
	,	m_uItemCount( 0 )
{
}

template<class T>
CArray<T>::CArray( PI_uint32 uSize )
	:	m_pItems( NULL )
	,	m_uGrowBy( 1 )
	,	m_uItemsAllocated( 0 )
	,	m_uItemCount( 0 )
{
	SetSize( uSize );
}

template<class T>
CArray<T>::CArray( const CArray<T> &arrToCopy )
	:	m_pItems( NULL )
	,	m_uGrowBy( 1 )
	,	m_uItemsAllocated( 0 )
	,	m_uItemCount( 0 )
{
	InsertAt( 0, arrToCopy );
}

template<class T>
CArray<T>::~CArray()
{
	RemoveAll();
}


template<class T>
bool CArray<T>::SetSize( PI_uint32 uSize )
{
	bool bReallocationNeeded = false;
	ASSERT( m_uItemCount <= m_uItemsAllocated );


	if( m_uItemCount < uSize )
	{
		if( m_uItemsAllocated < uSize )
		{
			PI_uint32 uAllocateExtra = m_uGrowBy = m_uGrowBy < 262144 ? m_uGrowBy << 2 : 262144;

			//
			// Check to see if our grow by is enough?
			if( m_uItemsAllocated + uAllocateExtra < uSize )
			{
				//
				//	Nope, so we allocate more
				uAllocateExtra = uSize - m_uItemsAllocated;
			}

			T *pItems = reinterpret_cast<T *>( new PI_byte[sizeof( T ) * ( m_uItemsAllocated + uAllocateExtra )] );

			if( m_uItemCount )
				MoveItemsNoMemOverlap( m_pItems, pItems, m_uItemCount );
			delete[] (PI_byte*)m_pItems;
			m_pItems = pItems;
			m_uItemsAllocated += uAllocateExtra;
			bReallocationNeeded = true;
		}

		//
		// Constuct the new items
		if( uSize > m_uItemCount )
		{
			PI_uint32 uItemsToConstruct = uSize - m_uItemCount;
			ConstructItems( &m_pItems[m_uItemCount], uItemsToConstruct );
			m_uItemCount = uSize;
		}

	}
	else
	{
		//
		//	Check to see if we need to reduce the size
		if( m_uItemCount > uSize )
		{
			//
			//	Get rid of excess elements
			RemoveAt( uSize, m_uItemCount - uSize );
		}
	}

	return bReallocationNeeded;
}

template<class T>
void CArray<T>::Add( const T &newItem )
{
	SetSize( m_uItemCount + 1 );
	m_pItems[m_uItemCount - 1] = newItem;
}

template<class T>
T &CArray<T>::Add()
{
	SetSize( m_uItemCount + 1 );
	return m_pItems[m_uItemCount - 1];
}

template<class T>
void CArray<T>::Add( const T *p, PI_uint32 uCount )
{
	InsertAt( GetSize(), p,  uCount );
}


template<class T>
void CArray<T>::InsertAt( PI_uint32 iPos, const T &newItem,  PI_uint32 uCount )
{
	ASSERT( uCount >= 1 );
	ASSERT( iPos <= m_uItemCount );
	PI_uint32 uItemsToMove = m_uItemCount - iPos;
	SetSize( m_uItemCount + uCount );

	//
	//	Move Elements above insertion point up one to make
	//	room for our new item
	MoveItemsMemOverlap( &m_pItems[iPos], &m_pItems[iPos + uCount], uItemsToMove );

	ConstructItems( &m_pItems[iPos], uCount );
	while( uCount-- )
	{
		m_pItems[iPos++] = newItem;
	}
}

template<class T>
void CArray<T>::InsertAt( PI_uint32 iPos, const CArray<T> &arrToCopy )
{
	InsertAt( iPos, arrToCopy.m_pItems, arrToCopy.m_uItemCount );
}

template<class T>
void CArray<T>::InsertAt( PI_uint32 iPos, const T *p, PI_uint32 uCount )
{
	if( p )
	{
		//
		// Cannot handle adding to its self
		ASSERT( m_pItems != p );
		ASSERT_VALID_READOBJPTR( p );
		//ASSERT( !IsBadReadPtr( p, sizeof( T ) * uCount ) );

		if( uCount == 0 )
			return;

		PI_uint32 uItemsToMove = m_uItemCount - iPos;

		ASSERT( iPos <= m_uItemCount );
		SetSize( m_uItemCount + uCount );

		//
		//	Move Elements above insertion point up one to make
		//	room for our new item
		MoveItemsMemOverlap( &m_pItems[iPos], &m_pItems[iPos + uCount], uItemsToMove );

		CopyItems( p, m_pItems + iPos, uCount );
	}
}


template<class T>
void CArray<T>::RemoveAll()
{
	DestructItems( m_pItems, m_uItemCount );
	delete [] reinterpret_cast<PI_byte *>( m_pItems );
	m_pItems = NULL;
	m_uGrowBy = 1;
	m_uItemsAllocated = 0;
	m_uItemCount = 0;
}

template<class T>
void CArray<T>::RemoveAt( PI_uint32 iPos, PI_uint32 uItems )
{
	ASSERT( iPos + uItems <= m_uItemCount );
	ASSERT( uItems > 0 );

	DestructItems( &m_pItems[iPos], uItems );

	//
	// Move Any items above removeal area down
	const PI_uint32 uItemsToMove = m_uItemCount - iPos - uItems;
	if( uItemsToMove )
		MoveItemsMemOverlap( &m_pItems[iPos + uItems], &m_pItems[iPos], uItemsToMove);
	m_uItemCount -= uItems;
}


template<class T>
CArray<T>	& CArray<T>::operator = ( const CArray<T> &rhs )
{
	if( this != &rhs )
	{
		RemoveAll();
		InsertAt( 0, rhs );
	}

	return *this;
}


template<class T>
T& CArray<T>::operator[]( PI_uint32 iItem )
{
	ASSERT( iItem < m_uItemCount );
	return m_pItems[iItem];
}

template<class T>
const T	& CArray<T>::operator[](PI_uint32 iItem) const
{
	ASSERT( iItem < m_uItemCount );
	return m_pItems[iItem];
}

template<class T>
PI_uint32 CArray<T>::GetSize() const
{
	return m_uItemCount;
}

template<class T>
const T *CArray<T>::GetData() const
{
	return m_pItems;
}

template<class T>
T *CArray<T>::GetData()
{
	return m_pItems;
}
