/*******************************************************************************
 * Copyright (c) 2010-02-22 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "saver.h"
#include "../btree_internal.h"

#include <btree/btree_func.h>

void SaveIncludes( SaverContext );
void SaveActions( SaverContext );
void SaveDecorators( SaverContext );
void SaveTrees( SaverContext );
void FlushBuffer( SaverContext );

#define SAVE_BUFFER_FLUSH_LIMIT (7 * 1024)

void* SaverContextGetExtra( SaverContext sc )
{
  return sc->m_Extra;
}

void SaverContextSetExtra( SaverContext sc, void* extra )
{
  sc->m_Extra = extra;
}

BehaviorTreeContext ParserContextGetBehaviorTreeContext( SaverContext sc )
{
  return sc->m_Tree;
}

void Save( SaverContext sc, SaverContextFunctions* funcs )
{
  sc->m_Funcs = *funcs;
  SaveIncludes( sc );
  SaveActions( sc );
  SaveDecorators( sc );
  SaveTrees( sc );
  FlushBuffer( sc );
}

void SaveIncludes( SaverContext sc )
{
  Include* i = sc->m_Tree->m_Includes;
  while( i )
  {
    const char* translated = sc->m_Funcs.m_Translate( sc, i->m_Name );
    StringBufferAppend( &sc->m_Buffer, "(include \"" );
    StringBufferAppend( &sc->m_Buffer, i->m_Name );
    StringBufferAppend( &sc->m_Buffer, "\")\n" );

    if( sc->m_Buffer.m_Size >= SAVE_BUFFER_FLUSH_LIMIT )
      FlushBuffer( sc );

    i = i->m_Next;
  }
  StringBufferAppend( &sc->m_Buffer, '\n' );
}

void SaveActions( SaverContext sc )
{

}

void SaveDecorators( SaverContext sc )
{

}

void SaveTrees( SaverContext sc )
{

}

void FlushBuffer( SaverContext sc )
{
  sc->m_Funcs.m_Write( sc, sc->m_Buffer.m_Str, sc->m_Buffer.m_Size );
  StringBufferClear( &sc->m_Buffer );
}

#undef SAVE_BUFFER_FLUSH_LIMIT
