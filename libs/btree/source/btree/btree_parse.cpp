/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <btree/btree_parse.h>
#include "../parser/common.h"


void* ParserContextGetExtra( ParserContext pc )
{
  return pc->m_Extra;
}

void ParserContextSetExtra( ParserContext pc, void* extra )
{
  pc->m_Extra = extra;
}

void ParserContextSetCurrent( ParserContext pc, const char* curr )
{
  pc->m_Current = curr;
}

const char* ParserContextGetCurrent( ParserContext pc )
{
  return pc->m_Current;
}

int ParserContextGetLineNo( ParserContext pc )
{
  return pc->m_LineNo;
}

int Parse( ParserContext pc, ParserContextFunctions* pcf )
{
  pc->m_LineNo      = 1;
  pc->m_Error       = pcf->m_Error;
  pc->m_Warning     = pcf->m_Warning;
  pc->m_Read        = pcf->m_Read;
  pc->m_Translate   = pcf->m_Translate;

  void* yyscanner = 0x0;
  yylex_init_extra( pc, &yyscanner );
  yyset_extra( pc, yyscanner );
  int r = yyparse( pc, yyscanner );
  yylex_destroy( yyscanner );

  pc->m_Error   = 0x0;
  pc->m_Warning = 0x0;
  pc->m_Read    = 0x0;

  return r;
}

BehaviorTreeContext ParserContextGetBehaviorTreeContext( ParserContext pc )
{
  return pc->m_Tree;
}
