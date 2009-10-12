/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BTREE_IMPL_H_
#define BTREE_IMPL_H_

#include <btree/btree_data.h>
#include "../parser/common.h"

#include "sym_table.h"
#include "object_pool.h"
#include "string_table.h"

struct StandardAllocator
{
    static void* Alloc( size_t size )
    {
        return malloc( size );
    }

    static void Free( void* ptr )
    {
        return free( ptr );
    }
};

struct HashPredicate
{
    inline bool operator() ( const hash_t l, const hash_t r ) const
    {
        return l < r;
    }
};

struct IntPredicate
{
    inline bool operator() ( int l, int r ) const
    {
        return l < r;
    }
    inline bool Equals( int l, int r ) const
    {
        return l == r;
    }
};

template< typename T >
struct HasIdPredicate
{
    inline bool operator() ( const T* l, const hash_t r ) const
    {
        return l->m_Id.m_Hash < r;
    }
    inline bool operator() ( const hash_t l, const T* r ) const
    {
        return l < r->m_Id.m_Hash;
    }
    inline bool operator() ( const T* l, const Identifier& r ) const
    {
        return l->m_Id.m_Hash < r.m_Hash;
    }
    inline bool operator() ( const Identifier& l, const T* r ) const
    {
        return l.m_Hash < r->m_Id.m_Hash;
    }
    inline bool operator() ( const T* l, const T* r ) const
    {
        return l->m_Id.m_Hash < r->m_Id.m_Hash;
    }

    inline bool Equals( const T* l, const Identifier& r ) const
    {
        return l->m_Id.m_Hash == r.m_Hash;
    }

    inline bool Equals( const T* l, const T* r ) const
    {
        return l == r;
    }

};

class BehaviorTreeImpl
{
public:

	BehaviorTreeImpl()
        : m_StringTable( 4096, 128 )
        , m_ParseFilePool( 16 )
    {}

    typedef TStringTable< StandardAllocator, hash_t, HashPredicate > StringTable;

    typedef TSymbolTable< int, IntPredicate >                     IntTable;
    typedef TSymbolTable< Node*, HasIdPredicate<Node> >           NodeTable;
    typedef TSymbolTable< Action*, HasIdPredicate<Action> >       ActionTable;
    typedef TSymbolTable< Decorator*, HasIdPredicate<Decorator> > DecoratorTable;

    typedef TObjectPool< ParseFile >                ParseFilePool;

    StringTable              m_StringTable;
    IntTable                 m_Defines;
    NodeTable                m_NodeTable;
    ActionTable              m_ActionTable;
    DecoratorTable           m_DecoratorTable;
    ParseFilePool            m_ParseFilePool;
};

#endif /*BTREE_IMPL_H_*/
