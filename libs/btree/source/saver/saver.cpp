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

void SaveIncludes( SaverContext );
void SaveActions( SaverContext );
void SaveDecorators( SaverContext );
void SaveTrees( SaverContext );

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
}

void SaveIncludes( SaverContext sc )
{

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
