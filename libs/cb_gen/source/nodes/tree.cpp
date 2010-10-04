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
#include "../node_generation_data.h"

#include <stdio.h>

namespace cb_gen {

int tree_setup( Node* n )
{
  Node* c = n->m_Grist.m_Tree.m_Tree->m_Root;
  if( int r = setup( c ) )
    return r;

  NodeGenerationNeeds* ngn = get_needs( c );
  printf( "Tree:      %s\n", n->m_Grist.m_Tree.m_Tree->m_Id.m_Text );
  printf( "Memory:    %d\n", ngn->m_MemoryUse );
  printf( "Stack:     %d\n", ngn->m_StackUse );
  char reg_use_str[ 256 ];
  uint curr_char = 0;
  reg_use_str[0] = 0;
  for( uint i = 0; i < cb::gen_reg_count; ++i )
  {
    if( ngn->m_RegUseCounters[i] == 0 )
      continue;
    if( curr_char > 0 )
      curr_char += sprintf( &(reg_use_str[curr_char]), ", " );
    curr_char += sprintf( &(reg_use_str[curr_char]), "%u", i );
  }
  if( curr_char > 0 )
    printf( "Registers: %s\n", reg_use_str );
  return 0;
}

}
