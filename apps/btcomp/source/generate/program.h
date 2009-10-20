/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <common/types.h>
#include <callback/instructions.h>
#include <callback/callback.h>
#include <btree/btree_data.h>
#include <vector>

struct Program;

class CodeSection
{
public:

    CodeSection();

    typedef uint32 TIn;

    void    SetGenerateDebugInfo( bool onoff );

    void    Print( FILE* outFile ) const;

    int     Count() const;
    void    Push( TIn inst, TIn A1, TIn A2, TIn A3 );

    void    SetA1( int i, TIn A1 );
    void    SetA2( int i, TIn A2 );
    void    SetA3( int i, TIn A3 );

    bool    Save( FILE* outFile, bool swapEndian ) const;

    void    PushDebugScope( Program* p, Node* n, callback::NodeAction action );
    void    PopDebugScope( Program* p, Node* n, callback::NodeAction action );

private:

    callback::VMIType SafeConvert( TIn i ) const;

    typedef std::vector<callback::Instruction> Instructions;
    Instructions m_Inst;
    bool         m_DebugInfo;
};

class BSSSection
{
public:

    BSSSection();

    void Print( FILE* outFile );
    int Push( int size, int alignment = 4 );

    int Size() const { return m_bss; }

private:

    int m_bss;
};

class DataSection
{
public:

    void Print( FILE* outFile );

    int PushInteger( int value );
    int PushFloat( float value );
    int PushString( const char* str );

    bool Save( FILE* outFile, bool swapEndian ) const;

private:

    int PushData( const char* data, int count );

    enum DataType
    {
        E_DT_INTEGER,
        E_DT_FLOAT,
        E_DT_STRING
    };

    struct MetaData
    {
        int m_Type;
        int m_Index;
    };

    struct StringLookup
    {
        int     m_Index;
        hash_t  m_Hash;
    };

    struct StringLookupPredicate
    {
        bool operator () ( const StringLookup& lhs, const StringLookup& rhs ) const
        {
            return lhs.m_Hash < rhs.m_Hash;
        }

        bool operator() ( hash_t lhs, const StringLookup& rhs ) const
        {
            return lhs < rhs.m_Hash;
        }

        bool operator() ( const StringLookup& lhs, hash_t rhs ) const
        {
            return lhs.m_Hash < rhs;
        }
    };

    typedef std::vector<char>          DataList;
    typedef std::vector<MetaData>      MetaDataList;
    typedef std::vector<StringLookup>  StringTable;

    DataList     m_Data;
    MetaDataList m_Meta;
    StringTable  m_String;
};

struct Program
{
	int m_bss_Header;
	int m_bss_Return;
	CodeSection m_I;
	BSSSection	m_B;
	DataSection m_D;
};

int setup_before_generate( Node* n, Program* p );
int teardown_after_generate( Node* n, Program* p );

int generate_program( Node* root, Program* p );

int print_program( FILE* outfile, Program* p );

int save_program( FILE* outfile, bool swapEndian, Program* p );

#endif /*PROGRAM_H_INCLUDED*/
