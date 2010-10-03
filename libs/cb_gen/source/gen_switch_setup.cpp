/*******************************************************************************
 * Copyright (c) 2010-09-20 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "gen_switch.h"
#include "nodes/sequence.h"
#include "nodes/selector.h"
#include "nodes/parallel.h"
#include "nodes/dyn_selector.h"
#include "nodes/tree.h"
#include "nodes/action.h"
#include "nodes/decorator.h"

#include <btree/btree.h>


namespace cb_gen {

int STUB_memory_needs( Node* )
{
  return -1;
}
int STUB_setup( Node* )
{
  return -1;
}
int STUB_teardown( Node* )
{
  return -1;
}
int STUB_gen_con( Node* )
{
  return -1;
}
int STUB_gen_exe( Node* )
{
  return -1;
}
int STUB_gen_des( Node* )
{
  return -1;
}

NodeGenerationNeeds* STUP_get_needs( Node* n )
{
  return (NodeGenerationNeeds*)n->m_UserData;
}

  NodeTypeFunctions g_NodeTypeFunctions[E_MAX_GRIST_TYPES] =
  {
    {
      &STUB_memory_needs,
      &STUB_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &sequence_memory_needed,
      &sequence_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &selector_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &parallel_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &dyn_selector_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &STUB_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &STUB_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &STUB_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &tree_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
    {
      &STUB_memory_needs,
      &action_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &action_get_needs
    },
    {
      &STUB_memory_needs,
      &decorator_setup,
      &STUB_teardown,
      &STUB_gen_con,
      &STUB_gen_exe,
      &STUB_gen_des,
      &STUP_get_needs
    },
  };

}
