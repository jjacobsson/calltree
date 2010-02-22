#include "btree_callbacks.h"
#include <btree/btree_func.h>
#include <malloc.h>

int read_file( ParserContext pc, char* buffer, int maxsize )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( !pi )
    return 0;
  if( pi->m_File->atEnd() )
    return 0;
  return (int)pi->m_File->read( buffer, maxsize );
}

void parser_error( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( pi )
  {
    fprintf( stdout, "%s(%d) : error : %s\n", pi->m_Name,
      ParserContextGetLineNo( pc ), msg );
  }
  else
  {
    fprintf( stdout, "<no file>(%d) : error : %s\n",
      ParserContextGetLineNo( pc ), msg );
  }
}

void parser_warning( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( pi )
  {
    fprintf( stdout, "%s(%d) : warning : %s\n", pi->m_Name,
      ParserContextGetLineNo( pc ), msg );
  }
  else
  {
    fprintf( stdout, "<no file>(%d) : warning : %s\n", ParserContextGetLineNo(
      pc ), msg );
  }
}

const char* parser_translate_include( ParserContext pc, const char* include )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  BehaviorTreeContext btc = ParserContextGetBehaviorTreeContext( pc );

  StringBuffer sb;

  Allocator a;
  a.m_Alloc = &allocate_memory;
  a.m_Free = &free_memory;

  StringBufferInit( a, &sb );

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
      StringBufferAppend( a, &sb, pi->m_Name, last + 1 );
  }

  StringBufferAppend( a, &sb, include );
  const char* ret = BehaviorTreeContextRegisterString( btc, sb.m_Str );
  StringBufferDestroy( a, &sb );

  return ret;
}

void* allocate_memory( mem_size_t size )
{
  return malloc( size );
}

void free_memory( void* ptr )
{
  if( ptr )
    free( ptr );
}
