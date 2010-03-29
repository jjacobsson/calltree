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

struct Program;

class Function
{
public:

  Function( BehaviorTreeContext btc, const char* name );

  typedef std::vector<cb::Instruction> Instructions;

  inline void add( const cb::Instruction& i )
  {
    m_Inst.push_back( i );
  }

  inline void add( unsigned char i, unsigned char a1, unsigned char a2, unsigned char a3 )
  {
    cb::Instruction inst;
    inst.i  = i;
    inst.a1 = a1;
    inst.a2 = a2;
    inst.a3 = a3;
    add( inst );
  }

  inline unsigned int size() const
  {
    return (unsigned int)m_Inst.size();
  }

  Identifier    m_Id;
  Instructions  m_Inst;
};

class FunctionTable
{
public:

  typedef std::vector<Function*> Functions;

  ~FunctionTable()
  {
    Functions::iterator it, it_e( m_Functions.end() );
    for( it = m_Functions.begin(); it != it_e; ++it )
      delete *it;
  }

  unsigned int find_function_index( const char* name );

  Functions m_Functions;
};

class JumpTargetTable
{
public:

  struct JumpTarget
  {
    Function*       m_Func;
    unsigned int    m_Inst;
  };

  unsigned int add( Function* f )
  {
    unsigned int r = m_Targets.size();
    JumpTarget jt;
    jt.m_Func = f;
    jt.m_Inst = 0x00000000;
    m_Targets.push_back( jt );
    return r;
  }

  void set_jump_target( unsigned int jump_target, unsigned int inst_offset )
  {
    m_Targets[jump_target].m_Inst = inst_offset;
  }

  typedef std::vector<JumpTarget> JumpTargets;
  JumpTargets m_Targets;
};

class CodeSection
{
public:

    CodeSection();

    typedef unsigned int TIn;

    void    SetGenerateDebugInfo( bool onoff );

    void    Setup( Program* p );

    void    Print( FILE* outFile ) const;

    int     Count() const;
    void    Push( TIn inst, TIn A1, TIn A2, TIn A3 );

    void    SetA1( int i, TIn A1 );
    void    SetA2( int i, TIn A2 );
    void    SetA3( int i, TIn A3 );

    bool    Save( FILE* outFile, bool swapEndian ) const;

    void    PushDebugScope( Program* p, Node* n, cb::NodeAction action );
    void    PopDebugScope( Program* p, Node* n, cb::NodeAction action );

private:

    typedef std::vector<cb::Instruction> Instructions;
    Instructions m_Inst;
    int          m_BssStart;
    bool         m_DebugInfo;
};

class BSSSection
{
public:

    BSSSection();

    void Print( FILE* outFile );
    int Push( int size, int alignment = 4 );

    int Size() const { return m_Max; }

    void PushScope();
    void PopScope();

private:
    int m_Max;
    int m_Current;
    std::vector<int> m_ScopeStack;
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

struct Program
{
	int m_bss_Header;
	int m_bss_Return;
	FunctionTable      m_Funcs;
	JumpTargetTable    m_Jumps;
	CodeSection m_I;
	BSSSection	m_B;
	DataSection m_D;
};


int setup_before_generate( Node* n, Program* p );

int teardown_after_generate( Node* n, Program* p );

int generate_program( BehaviorTreeContext btc, Program* p );

int print_program( FILE* outfile, Program* p );

int save_program( FILE* outfile, bool swapEndian, Program* p );

#endif /*PROGRAM_H_INCLUDED*/
