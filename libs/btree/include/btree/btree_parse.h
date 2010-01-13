/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BTREE_PARSE_HEADER_INCLUDED
#define BTREE_PARSE_HEADER_INCLUDED

typedef struct SParserContext* ParserContext;
typedef struct SBehaviorTreeContext* BehaviorTreeContext;

typedef void (*ParserErrorFunction)( ParserContext, const char* msg );
typedef void (*ParserWarningFunction)( ParserContext, const char* msg );
typedef int (*ParserFillBufferFunction)( ParserContext, char* buffer, int maxsize );
typedef const char* (*ParserTranslateIncludeFunction)( ParserContext, const char* );

struct ParserContextFunctions
{
  ParserErrorFunction               m_Error;
  ParserWarningFunction             m_Warning;
  ParserFillBufferFunction          m_Read;
  ParserTranslateIncludeFunction    m_Translate;
};

ParserContext ParserContextCreate( BehaviorTreeContext );

void ParserContextDestroy( ParserContext );

void* ParserContextGetExtra( ParserContext );
void ParserContextSetExtra( ParserContext, void* );

int ParserContextGetLineNo( ParserContext );

int Parse( ParserContext, ParserContextFunctions* );

#endif /*BTREE_PARSE_HEADER_INCLUDED*/
