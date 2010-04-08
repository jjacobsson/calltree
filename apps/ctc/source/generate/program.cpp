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

#include "program.h"
#include "nodes.h"
#include "endian.h"
#include "inst_text.h"

#include <btree/btree_func.h>
#include <other/lookup3.h>

#include <stdio.h>
#include <string.h>

#include <algorithm>

using namespace cb;

Function::Function( BehaviorTreeContext ctx, const char* name )
{
  init( &m_Id );
  m_Id.m_Hash = hashlittle( name );
  m_Id.m_Text = register_string( ctx, name, m_Id.m_Hash );
}

unsigned int FunctionTable::find_function_index( const char* name )
{
  return 0x00ffffff;
}

void print_instruction( FILE* outFile, const Instruction& inst, int i )
{
  /*
  fprintf( outFile, "0x%04x\t%-20s\t", i, g_InstructionNames[inst.m_I] );

  switch( inst.m_I )
  {
  case INST_JABC_R_EQUA_C:
    fprintf( outFile, "0x%04x    \t%-10s\t0x%04x", inst.m_A1,
      g_NodeReturnsNames[inst.m_A2], inst.m_A3 );
    break;
  case INST_JABC_R_DIFF_C:
    fprintf( outFile, "0x%04x    \t%-10s\t0x%04x", inst.m_A1,
      g_NodeReturnsNames[inst.m_A2], inst.m_A3 );
    break;
  case INST__STORE_C_IN_R:
    fprintf( outFile, "%-10s\t0x%04x    \t0x%04x",
      g_NodeReturnsNames[inst.m_A1], inst.m_A2, inst.m_A3 );
    break;
  default:
    fprintf( outFile, "0x%04x    \t0x%04x    \t0x%04x", inst.m_A1, inst.m_A2,
      inst.m_A3 );
    break;
  }
  */
}

CodeSection::CodeSection()
  : m_BssStart( 0 )
  , m_DebugInfo( false )
{
}

void CodeSection::SetGenerateDebugInfo( bool onoff )
{
  m_DebugInfo = onoff;
}

void CodeSection::Setup( Program* p )
{
  if( !m_DebugInfo )
    return;

  m_BssStart = p->m_B.Push( sizeof(DebugData), 4 );
}

void CodeSection::Print( FILE* outFile ) const
{
  /*
  int s = Count();
  fprintf( outFile, "%-6s\t%-20s\t%-10s\t%-10s\t%s\n", "Line", "Instruction",
    "A1", "A2", "A3" );
  for( int i = 0; i < s; ++i )
  {
    const Instruction& inst = m_Inst[i];
    print_instruction( outFile, inst, i );
    fprintf( outFile, "\n" );
  }
  fprintf( outFile, "\nCode:\t%d (%d instructions)\n", s * sizeof(Instruction),
    s );
  */
}

int CodeSection::Count() const
{
  return (int)m_Inst.size();
}

void CodeSection::Push( TIn inst, TIn A1, TIn A2, TIn A3 )
{
/*
  Instruction i;
  i.m_I = SafeConvert( inst );
  i.m_A1 = SafeConvert( A1 );
  i.m_A2 = SafeConvert( A2 );
  i.m_A3 = SafeConvert( A3 );
  m_Inst.push_back( i );
*/
}

void CodeSection::SetA1( int i, TIn A1 )
{
 // m_Inst[i].m_A1 = SafeConvert( A1 );
}
void CodeSection::SetA2( int i, TIn A2 )
{
 // m_Inst[i].m_A2 = SafeConvert( A2 );
}
void CodeSection::SetA3( int i, TIn A3 )
{
 // m_Inst[i].m_A3 = SafeConvert( A3 );
}

bool CodeSection::Save( FILE* outFile, bool swapEndian ) const
{
  if( m_Inst.empty() )
    return true;
/*
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
  size_t write = sizeof(Instruction) * s;
  size_t written = fwrite( &(t[0]), 1, write, outFile );
  return written == write;
*/
  return false;
}

int StringFromAction( Program* p, NodeAction action )
{
  return p->m_D.PushString( g_CBActionNames[ action ] );
}

int StringFromNode( Program* p, Node* n )
{
  const char* str = 0x0;
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_UNKOWN:
    str = "Unkown";
    break;
  case E_GRIST_SEQUENCE:
    str = "Sequence";
    break;
  case E_GRIST_SELECTOR:
    str = "Selector";
    break;
  case E_GRIST_PARALLEL:
    str = "Parallel";
    break;
  case E_GRIST_DYN_SELECTOR:
    str = "Dynamic Selector";
    break;
  case E_GRIST_SUCCEED:
    str = "Succeed";
    break;
  case E_GRIST_FAIL:
    str = "Fail";
    break;
  case E_GRIST_WORK:
    str = "Work";
    break;
  case E_GRIST_TREE:
    str = n->m_Grist.m_Tree.m_Tree->m_Id.m_Text;
    break;
  case E_GRIST_ACTION:
    str = n->m_Grist.m_Action.m_Action->m_Id.m_Text;
    break;
  case E_GRIST_DECORATOR:
    str = n->m_Grist.m_Decorator.m_Decorator->m_Id.m_Text;
    break;
  case E_MAX_GRIST_TYPES:
    str = "You are full of bananas.";
    break;
  }
  return p->m_D.PushString( str );
}

bool StandardNode( Node* n )
{
  switch( n->m_Grist.m_Type )
  {
  case E_GRIST_TREE:
  case E_GRIST_ACTION:
  case E_GRIST_DECORATOR:
    return false;
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_MAX_GRIST_TYPES:
    break;
  }
  return true;
}

void CodeSection::PushDebugScope( Program* p, Node* n, NodeAction action )
{
  if( !m_DebugInfo )
    return;
/*
  unsigned short flags = (unsigned short)(action & 0x0000000f);
  if( StandardNode( n ) )
    flags |= E_STANDARD_NODE;
  flags |= E_ENTER_SCOPE;

  Instruction i;
  i.m_I  = INST_STORE_PD_IN_B;
  i.m_A1 = m_BssStart + 0;
  i.m_A2 = StringFromAction( p, action );
  i.m_A3 = 0;
  m_Inst.push_back( i );
  i.m_I  = INST_STORE_PD_IN_B;
  i.m_A1 = m_BssStart + 4;
  i.m_A2 = StringFromNode( p, n );
  i.m_A3 = 0;
  m_Inst.push_back( i );
  i.m_I  = INST__STORE_C_IN_B;
  i.m_A1 = m_BssStart + 8;
  i.m_A3 = (n->m_NodeId&0xffff0000)>>16;
  i.m_A2 = (n->m_NodeId&0x0000ffff);
  m_Inst.push_back( i );
  i.m_I  = INST__STORE_C_IN_B;
  i.m_A1 = m_BssStart + 12;
  i.m_A2 = flags;
  i.m_A3 = 0;
  m_Inst.push_back( i );
  i.m_I  = INST_CALL_DEBUG_FN;
  i.m_A1 = m_BssStart;
  i.m_A2 = 0;
  i.m_A3 = 0;
  m_Inst.push_back( i );
*/
}

void CodeSection::PopDebugScope( Program* p, Node* n, NodeAction action )
{
  if( !m_DebugInfo )
    return;
/*
  unsigned short flags = (unsigned char)(action & 0x0000000f);
  if( StandardNode( n ) )
    flags |= E_STANDARD_NODE;
  flags |= E_EXIT_SCOPE;

  Instruction i;
  i.m_I  = INST_STORE_PD_IN_B;
  i.m_A1 = m_BssStart + 0;
  i.m_A2 = StringFromAction( p, action );
  i.m_A3 = 0;
  m_Inst.push_back( i );
  i.m_I  = INST_STORE_PD_IN_B;
  i.m_A1 = m_BssStart + 4;
  i.m_A2 = StringFromNode( p, n );
  i.m_A3 = 0;
  m_Inst.push_back( i );
  i.m_I  = INST__STORE_C_IN_B;
  i.m_A1 = m_BssStart + 8;
  i.m_A3 = (n->m_NodeId&0xffff0000)>>16;
  i.m_A2 = (n->m_NodeId&0x0000ffff);
  m_Inst.push_back( i );
  i.m_I  = INST__STORE_C_IN_B;
  i.m_A1 = m_BssStart + 12;
  i.m_A2 = flags;
  i.m_A3 = 0;
  m_Inst.push_back( i );
  i.m_I  = INST_CALL_DEBUG_FN;
  i.m_A1 = m_BssStart;
  i.m_A2 = 0;
  i.m_A3 = 0;
  m_Inst.push_back( i );
  */
}


BSSSection::BSSSection()
  : m_Max( 0 )
  , m_Current( 0 )
{
}

void BSSSection::Print( FILE* outFile )
{
  fprintf( outFile, ".bss:\t%d\n", m_Max );
}

int BSSSection::Push( int size, int align )
{
  if( (m_Current % align) != 0 )
    m_Current += (align - (m_Current % align));

  int r = m_Current;
  m_Current += size;

  m_Max = std::max( m_Max, m_Current );

  return r;
}

void BSSSection::PushScope()
{
  //m_ScopeStack.push_back( m_Current );
}

void BSSSection::PopScope()
{
  //m_Current = m_ScopeStack.back();
  //m_ScopeStack.pop_back();
}

void DataSection::Print( FILE* outFile )
{
  const char* type_strings[] =
    { "INTEGER", "FLOAT", "STRING" };

  MetaDataList::iterator it, it_e( m_Meta.end() );
  for( it = m_Meta.begin(); it != it_e; ++it )
  {
    fprintf( outFile, "\n0x%04x\t%s\t", (*it).m_Index,
      type_strings[(*it).m_Type] );
    switch( (*it).m_Type )
    {
    case E_DT_INTEGER:
      fprintf( outFile, "0x%08x", *(int*)(&m_Data[(*it).m_Index]) );
      break;
    case E_DT_FLOAT:
      fprintf( outFile, "%f", *(float*)(&m_Data[(*it).m_Index]) );
      break;
    case E_DT_STRING:
      fprintf( outFile, "%s", &m_Data[(*it).m_Index] );
      break;
    }
  }

  fprintf( outFile, "\n\nData Elements:\t%d\nData Size:\t%d\n", m_Meta.size(),
    m_Data.size() );

}

int DataSection::PushInteger( int value )
{
  int r = PushData( (char*)&value, sizeof(int) );
  MetaData md;
  md.m_Type = E_DT_INTEGER;
  md.m_Index = r;
  m_Meta.push_back( md );

  return r;
}

int DataSection::PushFloat( float value )
{
  int r = PushData( (char*)&value, sizeof(int) );
  MetaData md;
  md.m_Type = E_DT_FLOAT;
  md.m_Index = r;
  m_Meta.push_back( md );

  return r;
}

int DataSection::PushString( const char* str )
{
  hash_t hash = hashlittle( str );
  StringLookupPredicate pred;
  StringTable::iterator it = std::lower_bound( m_String.begin(),
    m_String.end(), hash, pred );

  if( it == m_String.end() || (*it).m_Hash != hash )
  {
    int r = PushData( str, strlen( str ) + 1 );
    MetaData md;
    md.m_Type = E_DT_STRING;
    md.m_Index = r;
    m_Meta.push_back( md );
    StringLookup sl;
    sl.m_Index = r;
    sl.m_Hash = hash;
    m_String.insert( it, sl );
    return r;
  }

  return (*it).m_Index;
}

int DataSection::Size() const
{
  return (int)m_Data.size();
}

bool DataSection::Save( FILE* outFile, bool swapEndian ) const
{
  if( m_Data.empty() )
    return true;

  DataList t( m_Data );
  if( swapEndian )
  {
    MetaDataList::const_iterator it, it_e( m_Meta.end() );
    for( it = m_Meta.begin(); it != it_e; ++it )
    {
      if( (*it).m_Type == E_DT_INTEGER || (*it).m_Type == E_DT_FLOAT )
        EndianSwap( *(int*)&(t[(*it).m_Index]) );
    }
  }

  size_t write = sizeof(char) * t.size();
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



int setup_before_generate( Node* n, Program* p )
{
  //Alloc storage area for bss header
  p->m_bss_Header = p->m_B.Push( sizeof(Context), 4 );
  //Alloc storage area for child-node return value.
  p->m_bss_Return = p->m_B.Push( sizeof(NodeReturns), 4 );

  p->m_I.Setup( p );

  return setup_gen( n, p );
}

int teardown_after_generate( Node* n, Program* p )
{
  return teardown_gen( n, p );
}

bool generate_functions( BehaviorTreeContext btc, Program* p, BehaviorTree* tree )
{
  return true;
}

#define ADD( I, A1, A2, A3 ) f->add(I, A1, A2, A3)

void load_jump_target_index_to_register( Function* f, unsigned char reg, unsigned int jt )
{
  if( jt <= 0xff )
  {
    ADD( iload, reg, ejt, (unsigned char)jt );
  }
  else if( jt <= 0xffff )
  {
    ADD( isetl, reg, (unsigned char)(jt&0x0000ff00)>>8, (unsigned char)(jt&0x000000ff) );
    ADD( iadd, reg, reg, ejt );
    ADD( iload, reg, reg, 0 );
  }
  else
  {
    ADD( iseth, reg, (unsigned char)(jt&0xff000000)>>24, (unsigned char)(jt&0x00ff0000)>>16 );
    ADD( iorl, reg, (unsigned char)(jt&0x0000ff00)>>8, (unsigned char)(jt&0x000000ff) );
    ADD( iadd, reg, reg, ejt );
    ADD( iload, reg, reg, 0 );
  }
}

int generate_program( BehaviorTreeContext btc, Program* p )
{
  if( !btc || !p )
  {
    // TODO: Report error, internal compiler error
    return -1;
  }

  NamedSymbol* mts = find_symbol( btc, "main" );

  if( !mts )
  {
    // TODO: Report error, main missing
    return -1;
  }

  if( mts->m_Type != E_ST_TREE )
  {
    // TODO: Report error, "main" is not a tree.
    return -1;
  }

  BehaviorTree* mt = mts->m_Symbol.m_Tree;

  // Put the entry function first in the function table...
  Function* f = new Function( btc, "__standard_entry" );
  p->m_Funcs.m_Functions.push_back( f );

  if( !generate_functions( btc, p, mt ) )
    return -1; // Error should already be reported.

  unsigned int tree_cons_func = p->m_Funcs.find_function_index( "main_construct" );
  unsigned int tree_exec_func = p->m_Funcs.find_function_index( "main_execute" );
  unsigned int tree_dest_func = p->m_Funcs.find_function_index( "main_destruct" );

  unsigned int jump_to_exec = p->m_Jumps.add( f );
  unsigned int jump_to_exit = p->m_Jumps.add( f );

  // Set r0 to ACT_CONSTRUCT
  ADD( isetl, er0, 0, ACT_CONSTRUCT );
  // Load the current tree state from memory into r1
  ADD( iload, er1, ems, 0 );
  //Generate instructions to store jump_to_exec in er2, respecting the size of jump_to_exec
  load_jump_target_index_to_register( f, er2, jump_to_exec );
  // Jump to r2 if r0 != r1
  ADD( ijne, er0, er1, er2 );
  // Call the tree's construction function
  ADD( icall, (unsigned char)(tree_cons_func&0x00ff0000)>>16, (unsigned char)(tree_cons_func&0x0000ff00)>>8, (unsigned char)(tree_cons_func&0x000000ff) );
  // Set r0 to the current tree state (ACT_EXECUTE)
  ADD( isetl, er0, 0, ACT_EXECUTE );
  // Store the state in memory
  ADD( istore, ems, 0, er0 );

  p->m_Jumps.set_jump_target( jump_to_exec, f->size() );


/*
  int err;

  //Jump past construction code if tree is already running
  p->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, E_NODE_WORKING, p->m_bss_Return );

  //Generate tree construction code
  if( (err = gen_con( n, p )) != 0 )
    return err;

  //Patch jump past construction code instruction
  p->m_I.SetA1( 0, p->m_I.Count() );

  //Generate tree execution code
  if( (err = gen_exe( n, p )) != 0 )
    return err;

  //Store return value in bss.
  p->m_I.Push( INST__STORE_R_IN_B, p->m_bss_Return, 0, 0 );

  //Jump past destruciton code if tree is running
  int patch_jump_out = p->m_I.Count();
  p->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_WORKING, 0 );

  //Generate destruction code
  if( (err = gen_des( n, p )) != 0 )
    return err;

  //Patch jump past destruction code instruction
  p->m_I.SetA1( patch_jump_out, p->m_I.Count() );

  //Suspend execution
  p->m_I.Push( INST_______SUSPEND, 0, 0, 0 );
*/
  return 0;
}

#undef ADD

int print_program( FILE* outFile, Program* p )
{
  p->m_I.Print( outFile );
  p->m_B.Print( outFile );
  p->m_D.Print( outFile );
  return 0;
}

int save_program( FILE* outFile, bool swapEndian, Program* p )
{
  return -1;
  /*
  ProgramHeader h;
  h.m_IC = p->m_I.Count();
  h.m_DS = p->m_D.Size();
  h.m_BS = p->m_B.Size();

  if( swapEndian )
  {
    EndianSwap( h.m_IC );
    EndianSwap( h.m_DS );
    EndianSwap( h.m_BS );
  }
  size_t write = sizeof(ProgramHeader);
  size_t written = fwrite( &h, 1, write, outFile );
  if( written != write )
    return -1;
  if( !p->m_I.Save( outFile, swapEndian ) )
    return -1;
  if( !p->m_D.Save( outFile, swapEndian ) )
    return -1;
  return 0;
  */
}
