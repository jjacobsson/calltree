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

#ifndef CB_GEN_NODE_GENERATION_DATA_H_INCLUDED
#define CB_GEN_NODE_GENERATION_DATA_H_INCLUDED

#include <callback/callback.h>
#include <cb_gen/cb_gen.h>

namespace cb_gen {

struct NodeGenerationNeeds
{
  uint m_RegUseCounters[cb::gen_reg_count]; //!< How many simultaneous uses of any given register in this node.
  uint m_MemoryUse;                         //!< How many bytes of memory needed for this node.
  uint m_StackUse;                          //!< How many bytes of stack needed for this node.
};

struct NodeGenerationState
{
  bool  m_RegSpilled[cb::gen_reg_count];
  uint  m_MemoryUse;
  uint  m_StackUse;
};

void init( NodeGenerationNeeds& needs );
void init( NodeGenerationState& state );

void accumulate_register_use_counters( NodeGenerationNeeds& my_needs, const NodeGenerationNeeds& child_needs );
uchar allocate_register( NodeGenerationNeeds& my_needs );

}

#endif
