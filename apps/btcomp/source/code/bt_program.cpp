/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "bt_program.h"
#include <callback/instructions.h>
#include <callback/callback.h>
#include "../compiler/node.h"
#include "../compiler/node_grist.h"
#include "../compiler/identifier.h"

#include "../btree/endian.h"
#include "../btree/inst_text.h"
#include "../btree/btree.h"

#include <other/lookup3.h>

#include <stdio.h>
#include <string.h>

#include <algorithm>


using namespace callback;

void print_instruction( FILE* outFile, const Instruction& inst, int i )
{
    fprintf( outFile, "0x%04x\t%-20s\t", i, g_InstructionNames[inst.m_I] );

    switch( inst.m_I )
    {
    case INST_JABC_R_EQUA_C:
        fprintf( outFile, "0x%04x    \t%-10s\t0x%04x", inst.m_A1, g_NodeReturnsNames[inst.m_A2], inst.m_A3 );
        break;
    case INST_JABC_R_DIFF_C:
        fprintf( outFile, "0x%04x    \t%-10s\t0x%04x", inst.m_A1, g_NodeReturnsNames[inst.m_A2], inst.m_A3 );
        break;
    case INST__STORE_C_IN_R:
        fprintf( outFile, "%-10s\t0x%04x    \t0x%04x", g_NodeReturnsNames[inst.m_A1], inst.m_A2, inst.m_A3 );
        break;
    default:
        fprintf( outFile, "0x%04x    \t0x%04x    \t0x%04x", inst.m_A1, inst.m_A2, inst.m_A3 );
        break;
    }
}

CodeSection::CodeSection()
    : m_DebugInfo( false )
{}

void CodeSection::SetGenerateDebugInfo( bool onoff )
{
    m_DebugInfo = onoff;
}

void CodeSection::Print( FILE* outFile ) const
{
    int s = Count();
    fprintf( outFile, "%-6s\t%-20s\t%-10s\t%-10s\t%s\n", "Line", "Instruction", "A1", "A2", "A3" );
    for( int i = 0; i < s; ++i )
    {
        const Instruction& inst = m_Inst[i];
        print_instruction( outFile, inst, i );
        fprintf( outFile, "\n" );
    }
    fprintf( outFile, "\nCode:\t%d (%d instructions)\n", s * sizeof( Instruction ), s );
}

int CodeSection::Count() const
{
    return (int)m_Inst.size();
}

void CodeSection::Push( TIn inst, TIn A1, TIn A2, TIn A3 )
{
    Instruction i;
    i.m_I   = SafeConvert( inst );
    i.m_A1  = SafeConvert( A1 );
    i.m_A2  = SafeConvert( A2 );
    i.m_A3  = SafeConvert( A3 );
    m_Inst.push_back( i );
}

void CodeSection::SetA1( int i, TIn A1 )
{
    m_Inst[i].m_A1 = SafeConvert( A1 );
}
void CodeSection::SetA2( int i, TIn A2 )
{
    m_Inst[i].m_A2 = SafeConvert( A2 );
}
void CodeSection::SetA3( int i, TIn A3 )
{
    m_Inst[i].m_A3 = SafeConvert( A3 );
}

bool CodeSection::Save( FILE* outFile, bool swapEndian ) const
{
    Instructions t( m_Inst );
    size_t s = t.size();

    if( swapEndian )
    {
        for( size_t i = 0; i < s; ++i )
        {
            EndianSwap( t[i].m_I );
            EndianSwap( t[i].m_A1 );
            EndianSwap( t[i].m_A2 );
            EndianSwap( t[i].m_A3 );


        }
    }
    size_t write = sizeof( Instruction ) * s;
    size_t written = fwrite( &(t[0]), 1, write, outFile );
    return written == write;
}

void CodeSection::PushDebugScope( BehaviourTree* bt, Node* n, NodeAction action )
{
    if( !m_DebugInfo )
        return;
    char buff[ 2048 ];
    sprintf( buff, "%-10s%-50s(%d)\t%-10s", "Enter", n->m_Id->m_Text, n->m_Id->m_Line, g_CBActionNames[action] );
    int data = bt->m_D.PushString( buff );
    Push( INST_CALL_DEBUG_FN, data, 0, 0 );
}

void CodeSection::PopDebugScope( BehaviourTree* bt, Node* n, NodeAction action )
{
    if( !m_DebugInfo )
        return;
    char buff[ 2048 ];
    sprintf( buff, "%-10s%-50s(%d)\t%-10s", "Exit", n->m_Id->m_Text, n->m_Id->m_Line, g_CBActionNames[action] );
    int data = bt->m_D.PushString( buff );
    Push( INST_CALL_DEBUG_FN, data, 0, 0 );
}

VMIType CodeSection::SafeConvert( TIn i ) const
{
    return static_cast<VMIType>( i );
}

BSSSection::BSSSection()
    : m_bss( 0 )
{
}

void BSSSection::Print( FILE* outFile )
{
    fprintf( outFile, ".bss:\t%d\n", m_bss );
}

int BSSSection::Push( int size, int align )
{
    if( (m_bss % align) != 0 )
        m_bss += (align - (m_bss % align));

    int r = m_bss;
    m_bss += size;
    return r;
}

void DataSection::Print( FILE* outFile )
{
    const char* type_strings[] = {
        "INTEGER",
        "FLOAT",
        "STRING"
    };

    MetaDataList::iterator it, it_e( m_Meta.end() );
    for( it = m_Meta.begin(); it != it_e; ++it )
    {
        fprintf( outFile, "\n0x%04x\t%s\t", (*it).m_Index, type_strings[(*it).m_Type] );
        switch( (*it).m_Type )
        {
        case E_DT_INTEGER:
            fprintf( outFile, "%d", *(int*)(&m_Data[(*it).m_Index]) );
            break;
        case E_DT_FLOAT:
            fprintf( outFile, "%f", *(float*)(&m_Data[(*it).m_Index]) );
            break;
        case E_DT_STRING:
            fprintf( outFile, "%s", &m_Data[(*it).m_Index] );
            break;
        }
    }

    fprintf( outFile, "\n\nData Elements:\t%d\nData Size:\t%d\n", m_Meta.size(), m_Data.size() );

}

int DataSection::PushInteger( int value )
{
    int r = PushData( (char*)&value, sizeof( int ) );
    MetaData md;
    md.m_Type   = E_DT_INTEGER;
    md.m_Index  = r;
    m_Meta.push_back( md );

    return r;
}

int DataSection::PushFloat( float value )
{
    int r = PushData( (char*)&value, sizeof( int ) );
    MetaData md;
    md.m_Type   = E_DT_FLOAT;
    md.m_Index  = r;
    m_Meta.push_back( md );

    return r;
}

int DataSection::PushString( const char* str )
{
    hash_t hash = hashlittle( str );
    StringLookupPredicate pred;
    StringTable::iterator it = std::lower_bound( m_String.begin(), m_String.end(), hash, pred );

    if( it == m_String.end() || (*it).m_Hash != hash )
    {
        int r = PushData( str, strlen( str ) + 1 );
        MetaData md;
        md.m_Type   = E_DT_STRING;
        md.m_Index  = r;
        m_Meta.push_back( md );
        StringLookup sl;
        sl.m_Index  = r;
        sl.m_Hash   = hash;
        m_String.insert( it, sl );
        return r;
    }

    return (*it).m_Index;
}

bool DataSection::Save( FILE* outFile, bool swapEndian ) const
{
    DataList t( m_Data );
    if( swapEndian )
    {
        MetaDataList::const_iterator it, it_e( m_Meta.end() );
        for( it = m_Meta.begin(); it != it_e; ++it )
        {
            if(  (*it).m_Type == E_DT_INTEGER || (*it).m_Type == E_DT_FLOAT )
                EndianSwap( *(int*)&(t[(*it).m_Index]) );
        }
    }

    size_t write = sizeof( char ) * t.size();
    size_t written = fwrite( &(t[0]), 1, write, outFile );
    return written == write;
}

int DataSection::PushData( const char* data, int count )
{
    int size = count;
    const int align = sizeof(void*);
    if( (size % align) != 0 )
        size += (align - (size % align));

    int start = m_Data.size();
    m_Data.resize( start + size );
    memcpy( &(m_Data[start]), data, count );

    return start;
}




