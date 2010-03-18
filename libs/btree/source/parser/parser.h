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

#ifndef PARSER_COMMON_H_
#define PARSER_COMMON_H_

#include <btree/btree.h>
#include "btree_bison.h"


struct SParserContext
{
  StringBuffer             m_Parsed;
  StringBuffer             m_Original;
  ParserContextFunctions   m_Funcs;
  Allocator                m_Allocator;
  unsigned int             m_LineNo;
  BehaviorTreeContext      m_Tree;
  void*                    m_Extra;
  const char*              m_Current;
};

#define YY_EXTRA_TYPE ParserContext

int  yylex( YYSTYPE*, void* );
int  yylex_init( void** );
int  yylex_init_extra( YY_EXTRA_TYPE, void** );
int  yylex_destroy( void* );
void yyset_extra( YY_EXTRA_TYPE, void* );
int  yyparse( YY_EXTRA_TYPE, void* );

void yyerror( ParserContext ctx, const char* msg );
void yywarning( ParserContext ctx, const char* msg );
void yyerror( ParserContext ctx, void*, const char* msg );
void yywarning( ParserContext ctx, void*, const char* msg );

#endif /* PARSER_COMMON_H_ */
