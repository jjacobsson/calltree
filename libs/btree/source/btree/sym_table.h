/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef SYM_TABLE_H_
#define SYM_TABLE_H_

#include <algorithm>
#include <stdlib.h>

template< typename T, typename PRED >
class TSymbolTable
{
public:

    TSymbolTable()
        : m_Begin( 0x0 )
        , m_Size( 0 )
        , m_Capacity( 0 )
        , m_Pred()
    {}

    ~TSymbolTable()
    {
        free( m_Begin );
    }

    typedef T*          iterator;
    typedef const T*    const_iterator;

    inline iterator begin() { return &(m_Begin[0]); }
    inline iterator end()   { return &(m_Begin[m_Size]); }

    inline const_iterator begin() const { return &(m_Begin[0]); }
    inline const_iterator end() const   { return &(m_Begin[m_Size]); }

    void Insert( T symbol )
    {
        if( m_Size == m_Capacity )
            Grow();

        iterator it = std::lower_bound( begin(), end(), symbol, m_Pred );
        if( it == end() || !m_Pred.Equals( *it, symbol ) )
        {
            memmove( it + 1, it, (end() - it) * sizeof( T ) );
            *it = symbol;
            ++m_Size;
        }
    }

    template< typename ID >
    T Find( ID id ) const
    {
        if( !m_Begin )
            return 0x0;

        const_iterator it = std::lower_bound( begin(), end(), id, m_Pred );
        if( it != end() && m_Pred.Equals( *it, id ) )
            return *(const_cast<iterator>(it));
        return 0x0;
    }

    template< typename ID >
    bool Exists( ID id ) const
    {
        if( !m_Begin )
            return false;

        const_iterator it = std::lower_bound( begin(), end(), id, m_Pred );
        if( it != end() && m_Pred.Equals( *it, id ) )
            return true;
        return false;
    }


    template< typename ID >
    void Erase( ID id )
    {
        if( !m_Begin )
            return;

        iterator it = std::lower_bound( begin(), end(), id, m_Pred );
        if( it != end() && m_Pred.Equals( *it, id ) )
        {
            memmove( it, it + 1, (end() - (it+1)) * sizeof( T ) );
            --m_Size;
        }
    }

private:

    void Grow()
    {
        m_Capacity += 128;
        const size_t alloc_size = sizeof(T*) * m_Capacity;
        T* new_buffer = (T*)malloc( alloc_size );

        memset( new_buffer, 0xcd, alloc_size );

        if( m_Size > 0 )
            memcpy( new_buffer, m_Begin, sizeof(T*) * (m_Size) );

        if( m_Begin )
            free( m_Begin );

        m_Begin = new_buffer;
    }

    T*      m_Begin;
    size_t  m_Size;
    size_t  m_Capacity;

    const PRED  m_Pred;
};

#endif /*SYM_TABLE_H_*/
