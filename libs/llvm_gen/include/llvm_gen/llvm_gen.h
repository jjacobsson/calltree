/*******************************************************************************
 * Copyright (c) 2010-04-15 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef LLVM_GEN_H_INCLUDED
#define LLVM_GEN_H_INCLUDED

#include <btree/btree.h>

namespace llvm
{
class Module;
}

namespace llvm_gen {

struct Program
{
  llvm::Module* m_Module;
};

void init( Program* );
void destroy( Program* );
void generate( BehaviorTreeContext, Program* );

}

#endif /* LLVM_GEN_H_INCLUDED */
