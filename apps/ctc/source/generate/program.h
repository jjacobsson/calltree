/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <callback/callback.h>
#include <btree/btree_data.h>

#include <vector>

#include <stdio.h>

struct Program;

class CodeSection
{
public:

    CodeSection();

    typedef unsigned int TIn;

    void    SetGenerateDebugInfo( int debug_level );

    void    Setup( Program* p );

    void    Print( FILE* outFile, Program* p ) const;

    int     Count() const;
    void    Push( TIn inst, TIn A1, TIn A2, TIn A3 );

    void    SetA1( int i, TIn A1 );
    void    SetA2( int i, TIn A2 );
    void    SetA3( int i, TIn A3 );

    bool    Save( FILE* outFile, bool swapEndian ) const;

    void    PushDebugScope( Program* p, Node* n, cb::NodeAction action, int dbg_lvl );
    void    PopDebugScope( Program* p, Node* n, cb::NodeAction action, int dbg_lvl );

private:

    unsigned int SafeConvert( TIn i ) const;

    typedef std::vector<cb::Instruction> Instructions;
    Instructions m_Inst;
    int          m_DebugLevel;
};

class DataSection
{
public:

    void Print( FILE* outFile );

    int PushInteger( int value );
    int PushFloat( float value );
    int PushString( const char* str );

    int Size() const;

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

struct BehaviorTreeList
{
  BehaviorTreeList* m_Next;
  BehaviorTree*     m_Tree;
  int               m_FirstInst;
};

struct Program
{
	CodeSection  m_I;
	DataSection  m_D;
	unsigned int m_Memory;
	BehaviorTreeContext m_Context;
	BehaviorTreeList* m_First;
};

int setup( BehaviorTreeContext ctx, Program* p );
int teardown( Program* p );
int generate( Program* p );

int print_program( FILE* outfile, Program* p );

int save_program( FILE* outfile, bool swapEndian, Program* p );

#endif /*PROGRAM_H_INCLUDED*/
