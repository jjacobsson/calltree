/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_

#include <stdlib.h>

class ObjectPoolInternal
{
public:

    ObjectPoolInternal( const size_t type_size, const size_t block_size )
        : m_TypeSize( type_size > sizeof( SObject ) ? type_size : sizeof( SObject ) )
        , m_BlockSize( block_size )
    {
        m_FirstBlock    = AllocateBlock();
        m_FirstFree     = m_FirstBlock->m_First;
    }

    ~ObjectPoolInternal()
    {
        SBlock* b = m_FirstBlock;
        while( b )
        {
            m_FirstBlock = b->m_Next;
            free( b );
            b = m_FirstBlock;
        }
    }

    void* Alloc()
    {
        SObject* o( m_FirstFree );
        if( o )
        {
            m_FirstFree = o->m_Next;
        }
        else
        {
            SBlock* t       = AllocateBlock();
            t->m_Next       = m_FirstBlock;
            m_FirstBlock    = t;
            o               = m_FirstBlock->m_First;
            m_FirstFree     = o->m_Next;
        }

        //Placement new, causes default constructor to be called if there is one.
        //new (o->m_Payload) T;

        return (void*)(o);
    }

    void Free( void* ptr )
    {
        //Early out on null argument
        if( !ptr ) return;

        //Explicit call to destructor
        //ptr->~T();

        //Cast to SObject
        SObject* o  = (SObject*)(ptr);
        //Scrub the payload memory
        memset( o, 0xcd, m_TypeSize );
        //Set next pointer to first free
        o->m_Next   = m_FirstFree;
        //Set first free to freed object
        m_FirstFree = o;
    }

private:

    struct SObject
    {
        SObject*    m_Next;
    };

    struct SBlock
    {
        SBlock*     m_Next;
        SObject*    m_First;
    };

    SBlock* AllocateBlock()
    {
        const size_t alloc_size = sizeof( SBlock ) + m_TypeSize * m_BlockSize;

        //Allocate new block info struct
        char* memory = (char*)malloc( alloc_size );

        //Scrub the memory
        memset( memory, 0xcd, alloc_size );

        SBlock* retVal = (SBlock*)memory;

        //Set m_Next
        retVal->m_Next = 0x0;

        //Set m_First
        retVal->m_First = (SObject*)(((char*)retVal) + sizeof( SBlock ));

        //Temporary to make it easier to setup the next pointers
        SObject* tmp = retVal->m_First;

        //Form a new free list by linking the memory chunks togheter.
        for( size_t i = 0; i < m_BlockSize-1; ++i )
        {
            tmp->m_Next = (SObject*)(((char*)tmp) + m_TypeSize );
            tmp = tmp->m_Next;
        }

        //Set the last next pointer to null
        tmp->m_Next = 0x0;

        //Return the Block info struct
        return retVal;
    }

    ObjectPoolInternal( const ObjectPoolInternal& o );
    const ObjectPoolInternal& operator = ( const ObjectPoolInternal& o );


    SBlock*         m_FirstBlock;
    SObject*        m_FirstFree;
    const size_t    m_TypeSize;
    const size_t    m_BlockSize;
};

template< typename T >
class TObjectPool
{
public:

    inline TObjectPool( size_t block_size )
        : m_Pool( sizeof( T ), block_size )
    {}

    inline T* Alloc()
    {
        //Allocate the memory
        void* mem = m_Pool.Alloc();

        //Placement new constructor call on the memory
        T* t = new (mem) T;

        //Return allocated object
        return t;
    }

    inline void Free( T* ptr )
    {
        //Early out return on null pointer
        if( ptr == 0x0 ) return;

        //Explicit destructor call
        ptr->~T();

        //Free the memory
        m_Pool.Free( ptr );
    }

private:

    TObjectPool( const TObjectPool& o );
    const TObjectPool& operator = ( const TObjectPool& o );


    ObjectPoolInternal m_Pool;
};

#endif /*OBJECTPOOL_H_*/
