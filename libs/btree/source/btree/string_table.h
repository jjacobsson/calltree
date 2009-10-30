/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef STRING_TABLE_H_
#define STRING_TABLE_H_

#include <algorithm>
#include <string.h>

template< typename ALLOCATOR, typename HASH_T, typename PRED >
    class TStringTable
{
private:

    struct SBlock
    {
        SBlock* m_Next;
        int     m_Used;
        char*   m_Text;
    };

    struct SLookup
    {
        HASH_T  m_Hash;
        char*   m_Str;
    };

    struct SWrapPredicate
    {
        SWrapPredicate()
            : m_Pred()
        {}

        const PRED m_Pred;

        inline bool operator()( const SLookup& l, const SLookup& r ) const
        {
            return m_Pred( l.m_Hash, r.m_Hash );
        }
    };

public:

    TStringTable( int block_size, int hash_table_grow )
        : m_BlockSize( block_size )
        , m_HashGrow( hash_table_grow )
        , m_HashSize( 0 )
        , m_HashCapacity( 0 )
        , m_HashTable( 0x0 )
        , m_FirstBlock( 0x0 )
    {
        GrowHashTable();
        m_FirstBlock = AllocateChunk();
    }

    ~TStringTable()
    {
        SBlock* c = m_FirstBlock;
        while( c )
        {
            m_FirstBlock = c->m_Next;
            ALLOCATOR::Free( c );
            c = m_FirstBlock;
        }
        ALLOCATOR::Free( m_HashTable );
    }

    const char* PutString( HASH_T hash, const char* str )
    {
        SLookup hl;
        hl.m_Hash   = hash;
        SLookup* it = std::lower_bound( m_HashTable, m_HashTable + m_HashSize, hl, m_Pred );
        if( it != m_HashTable + m_HashSize && it->m_Hash == hash )
            return it->m_Str;

        int l = strlen( str );
        if( m_BlockSize - m_FirstBlock->m_Used < l + 1 )
        {
            SBlock* c       = AllocateChunk( l + 1 );
            c->m_Next       = m_FirstBlock;
            m_FirstBlock    = c;
        }

        hl.m_Str = &(m_FirstBlock->m_Text[m_FirstBlock->m_Used]);
        HashInsert( hl );

        memcpy( hl.m_Str, str, l + 1 );
        m_FirstBlock->m_Used += l + 1;

        return hl.m_Str;
    }

private:

    void HashInsert( SLookup& l )
    {
        if( m_HashSize == m_HashCapacity )
            GrowHashTable();

        SLookup* it = std::lower_bound( m_HashTable, m_HashTable + m_HashSize, l, m_Pred );

        if( it != m_HashTable + m_HashSize )
        {
            int count   = (m_HashSize * sizeof( SLookup )) - ((int)it-(int)m_HashTable);
            memmove( it + 1, it, count );
        }

        it->m_Hash  = l.m_Hash;
        it->m_Str   = l.m_Str;
        m_HashSize++;
    }

    void GrowHashTable()
    {
        int alloc   = sizeof( SLookup ) * m_HashCapacity + sizeof( SLookup ) * m_HashGrow;
        SLookup* n  = (SLookup*)ALLOCATOR::Alloc( alloc );
        if( m_HashTable )
            memcpy( n, m_HashTable, sizeof( SLookup ) * m_HashCapacity );

        SLookup* o       = m_HashTable;
        m_HashTable      = n;
        m_HashCapacity  += m_HashGrow;

        ALLOCATOR::Free( o );
    }

    SBlock* AllocateChunk( int minimum = 0 )
    {
        int alloc;

        if( m_BlockSize < minimum )
            alloc = sizeof( SBlock ) + minimum;
        else
            alloc = sizeof( SBlock ) + m_BlockSize;

        SBlock* c = (SBlock*)ALLOCATOR::Alloc( alloc );
        c->m_Next = 0x0;
        c->m_Used = 0;
        c->m_Text = (char*)(c) + sizeof( SBlock );

        return c;
    }

    int             m_BlockSize;
    int             m_HashGrow;
    int             m_HashSize;
    int             m_HashCapacity;
    SLookup*        m_HashTable;
    SBlock*         m_FirstBlock;
    SWrapPredicate  m_Pred;
};

#endif /*STRING_TABLE_H_*/
