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

#include "node_generation_data.h"

namespace cb_gen {

void init( NodeGenerationNeeds& needs )
{
  for( uint i = 0; i < cb::gen_reg_count; ++i )
    needs.m_RegUseCounters[i] = 0;
  needs.m_MemoryUse = 0;
  needs.m_StackUse  = 0;
}

void init( NodeGenerationState& state )
{
  for( uint i = 0; i < cb::gen_reg_count; ++i )
    state.m_RegSpilled[i] = false;
  state.m_MemoryUse = 0;
  state.m_StackUse  = 0;
}

void accumulate_register_use_counters( NodeGenerationNeeds& my_needs, const NodeGenerationNeeds& child_needs )
{
  for( uint i = 0; i < cb::gen_reg_count; ++i )
    my_needs.m_RegUseCounters[i] += child_needs.m_RegUseCounters[i];
}

uchar allocate_register( NodeGenerationNeeds& my_needs )
{
  uchar best = 0;
  uint best_count = ~0;
  for( uint i = 0; i < cb::gen_reg_count; ++i )
  {
    if( my_needs.m_RegUseCounters[i] <= best_count )
    {
      best = i;
      best_count = my_needs.m_RegUseCounters[i];
    }
  }
  my_needs.m_RegUseCounters[best]++;
  return best;
}

}
