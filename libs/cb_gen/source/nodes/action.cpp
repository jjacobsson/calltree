/*******************************************************************************
 * Copyright (c) 2010-10-01 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "../node_generation_data.h"

namespace cb_gen {

int action_setup( Node* n )
{
  NodeGenerationNeeds* ngn = new NodeGenerationNeeds;
  init( *ngn );
  uint reg = allocate_register( *ngn );
  n->m_UserData = ngn;
  return 0;
}

int action_teardown( Node*n )
{
  NodeGenerationNeeds* ngn = (NodeGenerationNeeds*)n->m_UserData;
  delete ngn;
  n->m_UserData = 0x0;
  return 0;
}

int action_gen_construct( Function* f, Node* n )
{
  return -1;
}

NodeGenerationNeeds* action_get_needs( Node* n )
{
  return (NodeGenerationNeeds*)n->m_UserData;
}

}
