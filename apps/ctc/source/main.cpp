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
#include "generate/program.h"
#include "xgml_printer.h"

FILE* g_outputFile = 0x0;
char* g_inputFileName = 0x0;
char* g_outputFileName = "a.out";
bool g_swapEndian = false;
char* g_asmFileName = 0x0;
char* g_xgmlFileName = 0x0;
bool g_debug = false;

int g_allocs = 0;
int g_frees = 0;

struct ParsingInfo
{
  FILE* m_File;
  const char* m_Name;
};

int read_file( ParserContext pc, char* buffer, int maxsize )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( !pi )
    return 0;
  if( feof( pi->m_File ) )
    return 0;
  return (int)fread( buffer, 1, maxsize, pi->m_File );
}

void parser_error( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( pi )
  {
    printf( "%s(%d) : error : %s\n", pi->m_Name, ParserContextGetLineNo( pc ),
      msg );
  }
  else
  {
    printf( "%s: error : %s\n", g_inputFileName, msg );
  }
}

void parser_warning( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( pi )
  {
    printf( "%s(%d): warning : %s\n", pi->m_Name,
      ParserContextGetLineNo( pc ), msg );
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
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  BehaviorTreeContext btc = ParserContextGetBehaviorTreeContext( pc );

  Allocator a;
  a.m_Alloc = &allocate_memory;
  a.m_Free = &free_memory;

  StringBuffer sb;
  StringBufferInit( a, &sb );

  if( pi->m_Name )
  {
    char backslash  = '\\';
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
      StringBufferAppend( &sb, pi->m_Name, last + 1 );
  }

  StringBufferAppend( &sb, include );
  const char* ret = BehaviorTreeContextRegisterString( btc, sb.m_Str );
  StringBufferDestroy( &sb );

  return ret;
}

int main( int argc, char** argv )
{
  GetOptContext ctx;
  init_getopt_context( &ctx );
  char c;

  while( (c = getopt( argc, argv, "?i:o:a:dex:", &ctx )) != -1 )
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
      g_swapEndian = true;
      break;
    case 'a':
      g_asmFileName = ctx.optarg;
      break;
    case 'd':
      g_debug = true;
      break;
    case 'x':
      g_xgmlFileName = ctx.optarg;
      break;
    case '?':
      fprintf( stdout, "calltree compiler Version 0.1\n\n" );
      fprintf( stdout, "Options:\n" );
      fprintf( stdout, "\t-i\tInput file\n" );
      fprintf( stdout, "\t-o\tOutput file (defaults to \"a.out\")\n" );
      fprintf( stdout,
        "\t-a\tOutput text file of generated callback instructions. (optional)\n" );
      fprintf( stdout, "\t-x\tOutput a xgml file of the parsed tree\n" );
      fprintf( stdout, "\t-e\tEndian swap the output file.\n" );
      fprintf( stdout, "\t-d\tGenerate debug info\n" );
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
    BehaviorTreeContext btc = BehaviorTreeContextCreate( a );

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
      printf(
        "%s(0): error : unable to open input file \"%s\" for reading.\n",
        g_inputFileName, pi.m_Name );
      returnCode = -1;
    }

    if( returnCode == 0 )
    {
      ParserContext pc = ParserContextCreate( btc );
      ParserContextSetExtra( pc, &pi );
      ParserContextSetCurrent( pc, pi.m_Name );
      returnCode = Parse( pc, &pcf );
      ParserContextDestroy( pc );
    }

    if( pi.m_File )
      fclose( pi.m_File );

    Include* include = BehaviorTreeContextGetFirstInclude( btc );
    while( include )
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

      ParserContext pc = ParserContextCreate( btc );
      ParserContextSetExtra( pc, &pi );
      ParserContextSetCurrent( pc, pi.m_Name );
      returnCode = Parse( pc, &pcf );
      ParserContextDestroy( pc );

      if( pi.m_File )
        fclose( pi.m_File );

      if( returnCode != 0 )
        break;

      include = include->m_Next;
    }

    NamedSymbol* main = BehaviorTreeContextFindSymbol( btc, hashlittle( "main" ) );
    if( !main || main->m_Type != E_ST_TREE
        || !main->m_Symbol.m_Tree->m_Declared )
    {
      printf( "%s(0): error: \"main\" tree has not been declared.\n",
        g_inputFileName );
      returnCode = -1;
    }

    if( returnCode == 0 )
    {
      Program p;
      p.m_I.SetGenerateDebugInfo( g_debug );

      setup_before_generate( main->m_Symbol.m_Tree->m_Root, &p );
      returnCode = generate_program( main->m_Symbol.m_Tree->m_Root, &p );
      teardown_after_generate( main->m_Symbol.m_Tree->m_Root, &p );

      if( returnCode != 0 )
      {
        printf( "%s(0): error: Internal compiler error.\n", g_inputFileName );
      }
      else
      {
        g_outputFile = fopen( g_outputFileName, "wb" );
        if( !g_outputFile )
        {
          printf( "error: Unable to open output file %s for writing.\n",
            g_outputFileName );
          returnCode = -2;
        }

        if( returnCode == 0 )
          returnCode = save_program( g_outputFile, g_swapEndian, &p );
        if( returnCode != 0 )
        {
          printf( "error: Failed to write output file %s.\n", g_outputFileName );
          returnCode = -5;
        }
      }

      if( returnCode == 0 && g_asmFileName )
      {
        FILE* asmFile = fopen( g_asmFileName, "w" );
        if( !asmFile )
        {
          printf( "warning: Unable to open assembly file %s for writing.\n",
            asmFile );
        }
        else
        {
          print_program( asmFile, &p );
          fclose( asmFile );
        }
      }

    }
    if( returnCode == 0 && g_xgmlFileName )
    {

      XGMLPrinter xgml_printer;
      xgml_printer.Visit( main->m_Symbol.m_Tree->m_Root );
      xgml_printer.Layout();
      FILE* xgml_file = fopen( g_xgmlFileName, "w" );
      if( xgml_file )
      {
        xgml_printer.Print( xgml_file );
        fclose( xgml_file );
      }
      else
      {
        printf( "warning: Unable to open xgml file \"%s\" for writing.\n",
          g_xgmlFileName );
      }
    }

    BehaviorTreeContextDestroy( btc );
    if( g_allocs - g_frees != 0 )
      printf( "Allocs: %d\nFrees:  %d\nDelta:  %d\n", g_allocs, g_frees,
        g_allocs - g_frees );
  }

  if( g_outputFile )
    fclose( g_outputFile );

  return returnCode;
}
