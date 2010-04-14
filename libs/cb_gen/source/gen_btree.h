/*******************************************************************************
 * Copyright (c) 2010-04-07 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef CB_GEN_GEN_BTREE_H_INCLUDED
#define CB_GEN_GEN_BTREE_H_INCLUDED

#include <cb_gen/cb_gen.h>

namespace cb_gen {

void create_functions( BehaviorTreeContext, Program* );

void gen_node_con( Function*, Node*, uint );
void gen_node_exe( Function*, Node*, uint );
void gen_node_des( Function*, Node*, uint );

int memory_needed( BehaviorTree* );
int memory_needed( Node* );

}

#endif /* CB_GEN_GEN_BTREE_H_INCLUDED */
