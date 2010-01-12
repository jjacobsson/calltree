/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef PARSER_COMMON_H_
#define PARSER_COMMON_H_

#include <btree/btree_parse.h>
#include <btree/btree.h>
#include "btree_bison.h"

struct StringBuffer
{
  char* m_Str;
  int   m_Size;
  int   m_Capacity;
};

struct SParserContext
{
  StringBuffer              m_Parsed;
  StringBuffer              m_Original;
  int                       m_LineNo;
  BehaviorTreeContext       m_Tree;
  ParserErrorFunction       m_Error;
  ParserWarningFunction     m_Warning;
  ParserFillBufferFunction  m_Read;
  void*                     m_Extra;
  AllocateMemoryFunc        m_Alloc;
  FreeMemoryFunc            m_Free;
};



#define YY_EXTRA_TYPE ParserContext

int  yylex( YYSTYPE*, void* );
int  yylex_init( void** );
int yylex_init_extra( YY_EXTRA_TYPE, void** );
int  yylex_destroy( void* );
void yyset_extra( YY_EXTRA_TYPE, void* );
int yyparse( YY_EXTRA_TYPE, void* );

void yyerror( ParserContext ctx, const char* msg );
void yywarning( ParserContext ctx, const char* msg );
void yyerror( ParserContext ctx, void*, const char* msg );
void yywarning( ParserContext ctx, void*, const char* msg );

void StringBufferInit( ParserContext, StringBuffer* );
void StringBufferAppend( ParserContext, StringBuffer*, char );
void StringBufferAppend( ParserContext, StringBuffer*, const char * );
void StringBufferClear( ParserContext, StringBuffer* );
void StringBufferDestroy( ParserContext, StringBuffer* );

#endif /* PARSER_COMMON_H_ */
