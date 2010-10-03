/*******************************************************************************
 * Copyright (c) 2010-09-17 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "gen_switch.h"
#include <btree/btree_data.h>

namespace cb_gen {

  int memory_needs( Node* n )
  {
    return g_NodeTypeFunctions[n->m_Grist.m_Type].memory_needs( n );
  }

  int setup( Node* n )
  {
    return g_NodeTypeFunctions[n->m_Grist.m_Type].setup( n );
  }

  int teardown( Node* n )
  {
    return g_NodeTypeFunctions[n->m_Grist.m_Type].teardown( n );
  }

  int gen_con( Node* n )
  {
    return g_NodeTypeFunctions[n->m_Grist.m_Type].gen_con( n );
  }

  int gen_exe( Node* n )
  {
    return g_NodeTypeFunctions[n->m_Grist.m_Type].gen_exe( n );
  }

  int gen_des( Node* n )
  {
    return g_NodeTypeFunctions[n->m_Grist.m_Type].gen_des( n );
  }

}
