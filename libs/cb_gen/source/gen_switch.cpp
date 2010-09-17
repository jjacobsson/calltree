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

#include "nodes/sequence.h"

namespace cb_gen {

  NodeTypeFunctions g_NodeTypeFunctions[E_MAX_GRIST_TYPES] =
  {
    {
      &sequence_memory_needed,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0
    }
  };

}
