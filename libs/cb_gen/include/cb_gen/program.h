/*******************************************************************************
 * Copyright (c) 2010-04-03 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <vector>

#include <btree/btree.h>

namespace cb_gen {

class Function;

class Program
{
public:

  Program( BehaviorTreeContext );

private:

  BehaviorTreeContext m_Context;

};

}

#endif /* PROGRAM_H_INCLUDED */
