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

#include <stdlib.h>
#include <other/getopt.h>
#include <other/lookup3.h>

#include <btree/btree.h>
#include <cb_gen/cb_gen.h>
#include "generate/program.h"

#include <string.h>

FILE* g_outputFile = 0x0;
char* g_inputFileName = 0x0;
char* g_outputFileName = 0x0;
bool g_swapEndian = false;
bool g_printIncludes = false;
char* g_asmFileName = 0x0;
char* g_outputHeaderName = 0x0;

char* g_asmFileNameMemory = 0x0;

int g_allocs = 0;
int g_frees = 0;

struct ParsingInfo
{
  FILE* m_File;
  const char* m_Name;
};

int print_header( FILE* outfile, const char* file_name, BehaviorTreeContext ctx );

int read_file( ParserContext pc, char* buffer, int maxsize );
void parser_error( ParserContext pc, const char* msg );
void parser_warning( ParserContext pc, const char* msg );
void* allocate_memory( mem_size_t size );
void free_memory( void* ptr );
const char* parser_translate_include( ParserContext pc, const char* include );

int main( int argc, char** argv )
{
  GetOptContext ctx;
  init_getopt_context( &ctx );
  char c;

  while( (c = getopt( argc, argv, "?i:o:a:de:x:lcrh:", &ctx )) != -1 )
  {
    switch( c )
    {
    case 'i':
      g_inputFileName = ctx.optarg;
      break;
    case 'o':
      g_outputFileName = ctx.optarg;
      break;
    case 'e':
      if( strcmp( ctx.optarg, "big" ) == 0 )
        g_swapEndian = true;
      else if( strcmp( ctx.optarg, "little" ) == 0 )
        g_swapEndian = false;
      else
      {
        fprintf( stdout, "error: unknown argument for option -e: %s\n",
          ctx.optarg );
        return -1;
      }
      break;
    case 'a':
      g_asmFileName = ctx.optarg;
      break;
    case 'l':
      g_printIncludes = true;
      break;
    case 'c':
      /* don't ask. */
      break;
    case 'h':
      g_outputHeaderName = ctx.optarg;
      break;
    case '?':
      fprintf( stdout, "calltree compiler Version 0.1\n\n" );
      fprintf( stdout, "Options:\n" );
      fprintf( stdout, "\t-i\tInput file. (required)\n" );
      fprintf( stdout, "\t-o\tOutput file. (optional)\n" );
      fprintf( stdout,
        "\t-a\tOutput text file of generated callback instructions. (optional)\n" );
      fprintf(
        stdout,
        "\t-e\tSpecify endian, \"little\" or \"big\" as argument. (optional, default is \"little\").\n" );
      fprintf(
        stdout,
        "\t-l\tPrint a list of all files that the input file is dependent of. (optional)\n" );
      fprintf( stdout, "\t-?\tPrint this message and exit.\n\n" );
      return 0;
      break;
    }
  }

  int returnCode = 0;

  if( !g_inputFileName )
  {
    returnCode = -1;
    printf( "error: No input file given.\n" );
  }

  if( returnCode == 0 )
  {
    Allocator a;
    a.m_Alloc = &allocate_memory;
    a.m_Free = &free_memory;
    BehaviorTreeContext btc = create_bt_context( a );

    ParserContextFunctions pcf;
    pcf.m_Read = &read_file;
    pcf.m_Error = &parser_error;
    pcf.m_Warning = &parser_warning;
    pcf.m_Translate = &parser_translate_include;

    ParsingInfo pi;
    pi.m_Name = g_inputFileName;
    pi.m_File = fopen( pi.m_Name, "r" );
    if( !pi.m_File )
    {
      printf( "%s(0): error : unable to open input file \"%s\" for reading.\n",
        g_inputFileName, pi.m_Name );
      returnCode = -1;
    }

    if( returnCode == 0 )
    {
      ParserContext pc = create_parser_context( btc );
      set_extra( pc, &pi );
      set_current( pc, pi.m_Name );
      returnCode = parse( pc, &pcf );
      destroy( pc );
    }

    if( pi.m_File )
      fclose( pi.m_File );

    Include* include = get_first_include( btc );
    while( returnCode == 0 && include )
    {
      pi.m_Name = include->m_Name;
      pi.m_File = fopen( pi.m_Name, "r" );
      if( !pi.m_File )
      {
        printf(
          "%s(%d): error : unable to open include file \"%s\" for reading.\n",
          include->m_Parent, include->m_LineNo, pi.m_Name );
        returnCode = -1;
        break;
      }

      ParserContext pc = create_parser_context( btc );
      set_extra( pc, &pi );
      set_current( pc, pi.m_Name );
      returnCode = parse( pc, &pcf );
      destroy( pc );

      if( pi.m_File )
        fclose( pi.m_File );

      if( returnCode != 0 )
        break;

      include = include->m_Next;
    }

    include = get_first_include( btc );
    while( returnCode == 0 && include && g_printIncludes )
    {
      fprintf( stdout, "%s\n", include->m_Name );
      include = include->m_Next;
    }

    if( g_outputHeaderName && returnCode == 0 )
    {
      FILE* header = fopen( g_outputHeaderName, "w" );
      if( !header )
      {
        printf( "error: Unable to open output file %s for writing.\n", g_outputHeaderName );
        returnCode = -1;
      }
      else
      {
        returnCode = print_header( header, g_inputFileName, btc );
        fclose( header );
      }
    }

    if( g_outputFileName && returnCode == 0 )
    {
//      unsigned int debug_hash = hashlittle( "debug_info" );
//      Parameter* debug_param = find_by_hash( get_options( btc ), debug_hash );
//      if( debug_param )
//        p.m_I.SetGenerateDebugInfo( as_integer( *debug_param ) );

      cb_gen::Program p;
      cb_gen::init( &p );
      cb_gen::generate( btc, &p );

      if( !g_asmFileName )
      {
        unsigned int hash = hashlittle( "force_asm" );
        Parameter* force_asm = find_by_hash( get_options( btc ), hash );
        if( force_asm && as_bool( *force_asm ) )
        {
          unsigned int len = strlen( g_outputFileName );
          g_asmFileNameMemory = (char*)malloc( len + 5 );
          memcpy( g_asmFileNameMemory, g_outputFileName, len );
          g_asmFileNameMemory[len + 0] = '.';
          g_asmFileNameMemory[len + 1] = 'a';
          g_asmFileNameMemory[len + 2] = 's';
          g_asmFileNameMemory[len + 3] = 'm';
          g_asmFileNameMemory[len + 4] = 0;
          g_asmFileName = g_asmFileNameMemory;
        }
      }

      if( returnCode == 0 && g_asmFileName )
      {
        FILE* asmFile = fopen( g_asmFileName, "w" );
        if( !asmFile )
        {
          printf( "warning: Unable to open assembly file %s for writing.\n",
            g_asmFileName );
        }
        else
        {
          //cb_gen::print_asm( asmFile, &p );
          fclose( asmFile );
        }
      }

      cb_gen::destroy( &p );
    }

    destroy( btc );
    if( g_allocs - g_frees != 0 )
      printf( "Allocs: %d\nFrees:  %d\nDelta:  %d\n", g_allocs, g_frees,
        g_allocs - g_frees );
  }

  if( g_asmFileNameMemory )
    free( g_asmFileNameMemory );

  if( g_outputFile )
    fclose( g_outputFile );

  return returnCode;
}

const char* get_string_from_parameter_list( Parameter* pl, unsigned int hash )
{
  Parameter* p = find_by_hash( pl, hash );
  if( !p || !safe_to_convert( p, E_VART_STRING ) )
    return 0x0;
  return as_string( *p )->m_Parsed;
}

void print_header_entry( FILE* f, const char* symbol, const char* name, unsigned int value )
{
  if( symbol )
  {
    char tmp[1024];
    sprintf( tmp, "%s%s", symbol, name );
    fprintf( f, "const unsigned int %-60s = 0x%08x;\n", tmp, value );
  }
  else
  {
    fprintf( f, "const unsigned int %-60s = 0x%08x;\n", name, value );
  }
}

int print_header( FILE* f, const char* file_name, BehaviorTreeContext ctx )
{
  Parameter* opts = get_options( ctx );

  unsigned int header_hash = hashlittle( "ctc_h_header" );
  unsigned int footer_hash = hashlittle( "ctc_h_footer" );
  unsigned int symbol_hash = hashlittle( "ctc_h_symbol_prefix" );
  unsigned int id_hash     = hashlittle( "id" );

  const char* header = get_string_from_parameter_list( opts, header_hash );
  const char* footer = get_string_from_parameter_list( opts, footer_hash );
  const char* symbol = get_string_from_parameter_list( opts, symbol_hash );
  fprintf( f, "/*\n * This file is auto generated by ctc from %s.\n * Manual edits will be lost when regenerated.\n */\n\n", file_name );


  if( header )
    fprintf( f, "%s\n\n", header );

  int count;
  NamedSymbol* ns = access_symbols( ctx, &count );
  for( int i = 0; i < count; ++i )
  {
    if( ns[i].m_Type == E_ST_ACTION )
    {
      Parameter* p = find_by_hash( ns[i].m_Symbol.m_Action->m_Options, id_hash );
      if( p && safe_to_convert( p, E_VART_INTEGER ) )
        print_header_entry( f, symbol, ns[i].m_Symbol.m_Action->m_Id.m_Text, as_integer( *p ) );
      else
        print_header_entry( f, symbol, ns[i].m_Symbol.m_Action->m_Id.m_Text, ns[i].m_Symbol.m_Action->m_Id.m_Hash );
    }
    else if( ns[i].m_Type == E_ST_DECORATOR )
    {
      Parameter* p = find_by_hash( ns[i].m_Symbol.m_Decorator->m_Options, id_hash );
      if( p && safe_to_convert( p, E_VART_INTEGER ) )
        print_header_entry( f, symbol, ns[i].m_Symbol.m_Decorator->m_Id.m_Text, as_integer( *p ) );
      else
        print_header_entry( f, symbol, ns[i].m_Symbol.m_Decorator->m_Id.m_Text, ns[i].m_Symbol.m_Action->m_Id.m_Hash );
    }
  }

  if( footer )
    fprintf( f, "\n%s\n", footer );

  return 0;
}

int read_file( ParserContext pc, char* buffer, int maxsize )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( !pi )
    return 0;
  if( feof( pi->m_File ) )
    return 0;
  return (int)fread( buffer, 1, maxsize, pi->m_File );
}

void parser_error( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( pi )
  {
    printf( "%s(%d) : error : %s\n", pi->m_Name, get_line_no( pc ),
      msg );
  }
  else
  {
    printf( "%s: error : %s\n", g_inputFileName, msg );
  }
}

void parser_warning( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( pi )
  {
    printf( "%s(%d): warning : %s\n", pi->m_Name, get_line_no( pc ),
      msg );
  }
  else
  {
    printf( "%s: warning : %s\n", g_inputFileName, msg );
  }
}

void* allocate_memory( mem_size_t size )
{
  g_allocs++;
  return malloc( size );
}

void free_memory( void* ptr )
{
  if( ptr )
  {
    g_frees++;
    free( ptr );
  }
}

const char* parser_translate_include( ParserContext pc, const char* include )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  BehaviorTreeContext btc = get_bt_context( pc );

  Allocator a;
  a.m_Alloc = &allocate_memory;
  a.m_Free = &free_memory;

  StringBuffer sb;
  init( a, &sb );

  if( pi->m_Name )
  {
    char backslash = '\\';
    char frontslash = '/';

    int s = 0, last = -1;
    const char* p = pi->m_Name;
    while( p && *p )
    {
      if( *p == backslash || *p == frontslash )
        last = s;
      ++p;
      ++s;
    }
    if( last != -1 )
      append( &sb, pi->m_Name, last + 1 );
  }

  append( &sb, include );
  const char* ret = register_string( btc, sb.m_Str );
  destroy( &sb );

  return ret;
}
