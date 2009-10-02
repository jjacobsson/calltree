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

#include "../code/bt_program.h"

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

class BehaviourTreeImpl
{
public:

    BehaviourTreeImpl()
        : m_StringTable( 4096, 128 )
        , m_NodeListPool( 128 )
        , m_SequenceGristPool( 16 )
        , m_SelectorGristPool( 16 )
        , m_ParallelGristPool( 16 )
        , m_DynamicSelectorGristPool( 16 )
        , m_DecoratorGristPool( 16 )
        , m_ActionGristPool( 64 )
        , m_ParseFilePool( 16 )
    {}

    typedef TStringTable< StandardAllocator, hash_t, HashPredicate > StringTable;

    typedef TSymbolTable< int, IntPredicate >                     IntTable;
    typedef TSymbolTable< Node*, HasIdPredicate<Node> >           NodeTable;
    typedef TSymbolTable< Action*, HasIdPredicate<Action> >       ActionTable;
    typedef TSymbolTable< Decorator*, HasIdPredicate<Decorator> > DecoratorTable;

    typedef TObjectPool< NodeList >                 NodeListPool;
    typedef TObjectPool< SequenceNodeGrist >        SequenceGristPool;
    typedef TObjectPool< SelectorNodeGrist >        SelectorGristPool;
    typedef TObjectPool< ParallelNodeGrist >        ParallelGristPool;
    typedef TObjectPool< DynamicSelectorNodeGrist > DynamicSelectorGristPool;
    typedef TObjectPool< DecoratorNodeGrist >       DecoratorGristPool;
    typedef TObjectPool< ActionNodeGrist >          ActionGristPool;
    typedef TObjectPool< ParseFile >                ParseFilePool;

    StringTable              m_StringTable;
    IntTable                 m_Defines;
    NodeTable                m_NodeTable;
    ActionTable              m_ActionTable;
    DecoratorTable           m_DecoratorTable;
    NodeListPool             m_NodeListPool;
    SequenceGristPool        m_SequenceGristPool;
    SelectorGristPool        m_SelectorGristPool;
    ParallelGristPool        m_ParallelGristPool;
    DynamicSelectorGristPool m_DynamicSelectorGristPool;
    DecoratorGristPool       m_DecoratorGristPool;
    ActionGristPool          m_ActionGristPool;
    ParseFilePool            m_ParseFilePool;

    CodeSection        m_I;
    DataSection        m_D;
    BSSSection         m_B;

};

#endif /*BTREE_IMPL_H_*/
