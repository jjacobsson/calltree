/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include "common.h"
#include <string.h>

void yyerror( ParserContext ctx, const char* msg )
{
  if( ctx->m_Error )
    ctx->m_Error( ctx, msg );
}

void yywarning( ParserContext ctx, const char* msg )
{
  if( ctx->m_Warning )
    ctx->m_Warning( ctx, msg );
}

void yyerror( SParserContext* ctx, void*, const char* msg )
{
  yyerror( ctx, msg );
}

void yywarning( SParserContext* ctx, void*, const char* msg )
{
  yywarning( ctx, msg );
}

void StringBufferGrow( ParserContext pc, StringBuffer* sb, int min )
{
  int ns = sb->m_Capacity + (128>min?128:min);
  char* t = (char*)pc->m_Alloc( ns );
  if( sb->m_Str )
  {
    if( sb->m_Size > 0 )
      memcpy( t, sb->m_Str, sb->m_Size + 1 );

    pc->m_Free( sb->m_Str );

    sb->m_Str       = t;
    sb->m_Capacity  = ns;
  }
  else
  {
    sb->m_Capacity  = ns;
    sb->m_Str       = t;
  }
}

void StringBufferInit( ParserContext pc, StringBuffer* sb )
{
  sb->m_Size        = 0;
  sb->m_Capacity    = 0;
  sb->m_Str         = 0x0;
  StringBufferGrow( pc, sb, 1024 );
  sb->m_Str[0]      = 0;
}

void StringBufferAppend( ParserContext pc, StringBuffer* sb, char c)
{
  if( sb->m_Size >= sb->m_Capacity - 1 )
    StringBufferGrow( pc, sb, 128 );

  sb->m_Str[sb->m_Size++]   = c;
  sb->m_Str[sb->m_Size]     = 0;
}

void StringBufferAppend( ParserContext pc, StringBuffer* sb, const char* str )
{
  int l = strlen( str );
  if( sb->m_Size >= sb->m_Capacity - (l + 1) )
    StringBufferGrow( pc, sb, l + 1 );
  memcpy( sb->m_Str + sb->m_Size, str, l + 1 );
}

void StringBufferClear( ParserContext pc, StringBuffer* sb )
{
  if( sb->m_Capacity <= 0 )
    return;
  sb->m_Size    = 0;
  sb->m_Str[0]  = 0;
}

void StringBufferDestroy( ParserContext pc, StringBuffer* sb )
{
  pc->m_Free( sb->m_Str );
  sb->m_Str         = 0x0;
  sb->m_Size        = 0;
  sb->m_Capacity    = 0;
}
