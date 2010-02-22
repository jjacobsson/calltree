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

#include <btree/btree_parse.h>
#include "parser/common.h"


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

  pc->m_Error       = 0x0;
  pc->m_Warning     = 0x0;
  pc->m_Read        = 0x0;
  pc->m_Translate   = 0x0;

  return r;
}

BehaviorTreeContext ParserContextGetBehaviorTreeContext( ParserContext pc )
{
  return pc->m_Tree;
}
