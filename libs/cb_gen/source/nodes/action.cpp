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

struct SActionNodeData
{
  NodeGenerationNeeds   m_Needs;
  uint                  m_ParamReg; //set to parameter values that then get pushed to the stack
};

int action_setup( Node* n )
{
  uint dec_count = count_elements( n->m_Grist.m_Action.m_Parameters );
  uint par_count = count_elements( n->m_Grist.m_Action.m_Action->m_Declarations );
  if( dec_count != par_count )
  {
    //TODO: ERROR MESSAGE!
    return -1;
  }

  SActionNodeData* nd = new SActionNodeData;
  n->m_UserData = nd;
  init( nd->m_Needs );

  if( par_count > 0 )
  {
    nd->m_ParamReg = allocate_register( nd->m_Needs );
    nd->m_Needs.m_StackUse += par_count * sizeof( uint );
  }

  return 0;
}

int action_teardown( Node*n )
{
  SActionNodeData* nd = (SActionNodeData*)n->m_UserData;
  delete nd;
  n->m_UserData = 0x0;
  return 0;
}

int action_gen_construct( Function* f, Node* n )
{
  return -1;
}

NodeGenerationNeeds* action_get_needs( Node* n )
{
  return &((SActionNodeData*)n->m_UserData)->m_Needs;
}

}
