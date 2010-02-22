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

#ifndef SAVER_H_INCLUDED
#define SAVER_H_INCLUDED

#include <btree/btree_data.h>

struct SSaverContext
{
  StringBuffer          m_Buffer;
  BehaviorTreeContext   m_Tree;
  SaverContextFunctions m_Funcs;
  void*                 m_Extra;
  Allocator             m_Allocator;
};

#endif /* SAVER_H_INCLUDED */
