/*******************************************************************************
 * Copyright (c) 2010-04-06 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef CB_GEN_TYPES_H_INCLUDED
#define CB_GEN_TYPES_H_INCLUDED

#include <vector>
#include <btree/btree.h>
#include <callback/callback.h>

namespace cb_gen {

struct Function;
struct Program;

struct FunctionEntry
{
  Function* m_Func;
  hash_t    m_Id;
};

typedef std::vector<Instruction> InstList;
typedef std::vector<FunctionEntry> FunctionList;

}

#endif /* CB_GEN_TYPES_H_INCLUDED */
