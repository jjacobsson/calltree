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

#include <btree/btree_func.h>
#include <other/lookup3.h>

#include <stdio.h>
#include <string.h>

#include <algorithm>

using namespace cb;

void print_instruction( FILE* outFile, const Instruction& inst, int g, int f )
{
/*
  char func[16], glob[16], a1[16], a2[16], a3[16];

  sprintf( func, "0x%04x", f );
  sprintf( glob, "0x%04x", g );
  sprintf( a1, "0x%04x", inst.m_A1 );
  sprintf( a2, "0x%04x", inst.m_A2 );
  sprintf( a3, "0x%04x", inst.m_A3 );
  fprintf( outFile, "%-8s%-8s%-20s", glob, func, g_InstructionNames[inst.m_I] );

  switch( inst.m_I )
  {
  case INST_JABC_R_EQUA_C:
    fprintf( outFile, "%-9s%-9s%-9s", a1, g_NodeReturnsNames[inst.m_A2], a3 );
    break;
  case INST_JABC_R_DIFF_C:
    fprintf( outFile, "%-9s%-9s%-9s", a1, g_NodeReturnsNames[inst.m_A2], a3 );
    break;
  case INST__STORE_C_IN_R:
    fprintf( outFile, "%-9s%-9s%-9s", g_NodeReturnsNames[inst.m_A1], a2, a3 );
    break;
  default:
    fprintf( outFile, "%-9s%-9s%-9s", a1, a2, a3 );
    break;
  }
*/
}

CodeSection::CodeSection()
  : m_DebugLevel( 0 )
{
}

void CodeSection::SetGenerateDebugInfo( int debug_level )
{
  m_DebugLevel = debug_level;
}

void CodeSection::Setup( Program* p )
{
}

void CodeSection::Print( FILE* outFile, Program* p ) const
{
  BehaviorTreeList* btl = p->m_First;

  int f = 0;
  int s = btl->m_FirstInst;

  fprintf( outFile, "__entry_stub\n" );
  fprintf( outFile, "%-8s%-8s%-20s%-9s%-9s%-9s\n", "Global", "Func",
    "Instruction", "A1", "A2", "A3" );

  f = 0;

  for( int i = 0; i < s; ++i, ++f )
  {
    const Instruction& inst = m_Inst[i];
    print_instruction( outFile, inst, i, f );
    fprintf( outFile, "\n" );
  }

  while( btl )
  {
    if( btl->m_Next )
      s = btl->m_Next->m_FirstInst;
    else
      s = Count();

    fprintf( outFile, "\n%s\n", btl->m_Tree->m_Id.m_Text );
    fprintf( outFile, "%-8s%-8s%-20s%-9s%-9s%-9s\n", "Global", "Func",
      "Instruction", "A1", "A2", "A3" );

    f = 0;

    for( int i = btl->m_FirstInst; i < s; ++i, ++f )
    {
      const Instruction& inst = m_Inst[i];
      print_instruction( outFile, inst, i, f );
      fprintf( outFile, "\n" );
    }
    btl = btl->m_Next;
  }

  fprintf( outFile, "\nCode:\t%d (%d instructions)\n", s * sizeof(Instruction),
    s );
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
  //m_Inst[i].m_A1 = SafeConvert( A1 );
}
void CodeSection::SetA2( int i, TIn A2 )
{
  //m_Inst[i].m_A2 = SafeConvert( A2 );
}
void CodeSection::SetA3( int i, TIn A3 )
{
  //m_Inst[i].m_A3 = SafeConvert( A3 );
}

bool CodeSection::Save( FILE* outFile, bool swapEndian ) const
{
  if( m_Inst.empty() )
    return true;

  Instructions t( m_Inst );
  size_t s = t.size();

  if( swapEndian )
  {
    for( size_t i = 0; i < s; ++i )
    {
//      EndianSwap( t[i].m_I );
//      EndianSwap( t[i].m_A1 );
//      EndianSwap( t[i].m_A2 );
//      EndianSwap( t[i].m_A3 );
    }
  }
  size_t write = sizeof(Instruction) * s;
  size_t written = fwrite( &(t[0]), 1, write, outFile );
  return written == write;
}

int StringFromAction( Program* p, NodeAction action )
{
  //return p->m_D.PushString( g_CBActionNames[action] );
  return -1;
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

void CodeSection::PushDebugScope( Program* p, Node* n, NodeAction action, int debug_level )
{
  if( m_DebugLevel < debug_level )
    return;
/*
  bool standard = StandardNode( n );
  bool composite = n->m_Grist.m_Type == E_GRIST_TREE;

  unsigned int flags = (unsigned int)(action & 0x0000000f);
  if( standard )
    flags |= E_STANDARD_NODE;
  if( composite )
    flags |= E_COMPOSITE_NODE;

  flags |= E_ENTER_SCOPE;

  unsigned int t;
  Instruction i;
  t = StringFromAction( p, action );
  i.m_I = INST_LOAD_REGISTRY;
  i.m_A1 = 0;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = StringFromNode( p, n );
  i.m_I = INST_LOAD_REGISTRY;
  i.m_A1 = 1;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = n->m_NodeId;
  i.m_I = INST__SET_REGISTRY;
  i.m_A1 = 2;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = flags;
  i.m_I = INST__SET_REGISTRY;
  i.m_A1 = 3;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = n->m_Locator.m_LineNo;
  i.m_I = INST__SET_REGISTRY;
  i.m_A1 = 4;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  i.m_I = INST_CALL_DEBUG_FN;
  i.m_A1 = 0;
  i.m_A2 = 0;
  i.m_A3 = 0;
  m_Inst.push_back( i );
*/
}

void CodeSection::PopDebugScope( Program* p, Node* n, NodeAction action, int debug_level )
{
  if( m_DebugLevel < debug_level )
    return;
/*
  bool standard = StandardNode( n );
  bool composite = n->m_Grist.m_Type == E_GRIST_TREE;

  unsigned int flags = (unsigned int)(action & 0x0000000f);
  if( standard )
    flags |= E_STANDARD_NODE;
  if( composite )
    flags |= E_COMPOSITE_NODE;

  flags |= E_EXIT_SCOPE;

  unsigned int t;
  Instruction i;
  t = StringFromAction( p, action );
  i.m_I = INST_LOAD_REGISTRY;
  i.m_A1 = 0;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = StringFromNode( p, n );
  i.m_I = INST_LOAD_REGISTRY;
  i.m_A1 = 1;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = n->m_NodeId;
  i.m_I = INST__SET_REGISTRY;
  i.m_A1 = 2;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = flags;
  i.m_I = INST__SET_REGISTRY;
  i.m_A1 = 3;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  t = n->m_Locator.m_LineNo;
  i.m_I = INST__SET_REGISTRY;
  i.m_A1 = 4;
  i.m_A2 = (unsigned short)((t & 0xffff0000) >> 16);
  i.m_A3 = (unsigned short)((t & 0x0000ffff));
  m_Inst.push_back( i );
  i.m_I = INST_CALL_DEBUG_FN;
  i.m_A1 = 0;
  i.m_A2 = 0;
  i.m_A3 = 0;
  m_Inst.push_back( i );
*/
}

unsigned int CodeSection::SafeConvert( TIn i ) const
{
  return static_cast<unsigned int> ( i );
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

int print_program( FILE* outFile, Program* p )
{
  p->m_I.Print( outFile, p );
  fprintf( outFile, "\nMemory: %u bytes.\n", p->m_Memory );
  p->m_D.Print( outFile );
  return 0;
}

int save_program( FILE* outFile, bool swapEndian, Program* p )
{
  ProgramHeader h;
//  h.m_IC = p->m_I.Count();
//  h.m_DS = p->m_D.Size();
//  h.m_BS = p->m_Memory;

  if( swapEndian )
  {
//    EndianSwap( h.m_IC );
//    EndianSwap( h.m_DS );
//    EndianSwap( h.m_BS );
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
}

void parser_error( ParserContext pc, const char* msg );
void parser_warning( ParserContext pc, const char* msg );

int setup( BehaviorTreeContext ctx, Program* p )
{
  NamedSymbol* main = find_symbol( ctx, hashlittle( "main" ) );

  if( !main || main->m_Type != E_ST_TREE || !main->m_Symbol.m_Tree->m_Declared )
  {
    parser_error( 0x0, "\"main\" tree has not been declared." );
    return -1;
  }
  else if( main->m_Symbol.m_Tree->m_Root == 0x0 )
  {
    parser_error( 0x0, "\"main\" contains zero node's." );
    return -1;
  }
  BehaviorTreeList* btl = new BehaviorTreeList;
  btl->m_Next = 0x0;
  btl->m_Tree = main->m_Symbol.m_Tree;
  p->m_First = btl;

  p->m_I.Setup( p );

  p->m_Memory = 0;
//  p->m_Memory += sizeof(BssHeader);
  p->m_Memory += sizeof(CallFrame);
  p->m_Memory += memory_need_btree( btl->m_Tree );

  while( btl )
  {
    int mem_alloc = gen_setup_btree( btl->m_Tree, p, 0 );
    int mem_needs = memory_need_btree( btl->m_Tree );
    if( mem_alloc < 0 || mem_alloc != mem_needs )
      return -1;

    btl = btl->m_Next;
  }

  return 0;
}

int teardown( Program* p )
{
  return 0;
}

int generate( Program* p )
{
  return -1;
/*
  if( p->m_First == 0x0 )
    return -1;

  p->m_I.Push( INST_SCRIPT_C, 2, 0, 0 );
  p->m_I.Push( INST_______SUSPEND, 0, 0, 0 );

  BehaviorTreeList* btl = p->m_First;
  while( btl )
  {
    btl->m_FirstInst = p->m_I.Count();
    if( gen_btree( btl->m_Tree, p ) != 0 )
      return -1;
    btl = btl->m_Next;
  }

  btl = p->m_First;
  while( btl )
  {
    patch_calls( btl->m_Tree->m_Root, p );
    btl = btl->m_Next;
  }

  return 0;
*/
}

