/*******************************************************************************
 * Copyright (c) 2010-10-04 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include <btree/btree.h>
#include "../gen_switch.h"

namespace cb_gen {

int tree_setup( Node* n )
{
  Node* c = n->m_Grist.m_Tree.m_Tree->m_Root;
  return setup( c );
}

}
