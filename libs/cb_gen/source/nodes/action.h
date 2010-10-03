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

#ifndef CB_GEN_NODES_ACTION_H_INCLUDED
#define CB_GEN_NODES_ACTION_H_INCLUDED

struct Node;

namespace cb_gen {

struct NodeGenerationNeeds;

  int action_setup( Node* n );
  int action_teardown( Node* n );
  NodeGenerationNeeds* action_get_needs( Node* n );

}

#endif
