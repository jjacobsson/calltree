/*******************************************************************************
 * Copyright (c) 2010-06-21 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef CB_GEN_GEN_SWITCH_H_INCLUDED
#define CB_GEN_GEN_SWITCH_H_INCLUDED

struct Node;

namespace cb_gen {

struct NodeGenerationNeeds;

typedef int (*Func_MemoryNeeds)( Node* );
typedef int (*Func_Setup)( Node* );
typedef int (*Func_Teardown)( Node* );
typedef int (*Func_GenConstruction)( Node* );
typedef int (*Func_GenExecution)( Node* );
typedef int (*Func_GenDestruction)( Node* );
typedef NodeGenerationNeeds* (*Func_GetNeeds)( Node* );

struct NodeTypeFunctions
{
  Func_MemoryNeeds     memory_needs;
  Func_Setup           setup;
  Func_Teardown        teardown;
  Func_GenConstruction gen_con;
  Func_GenExecution    gen_exe;
  Func_GenDestruction  gen_des;
  Func_GetNeeds        get_needs;
};

extern NodeTypeFunctions g_NodeTypeFunctions[];

int memory_needs( Node* );
int setup( Node* );
int teardown( Node* );
int gen_con( Node* );
int gen_exe( Node* );
int gen_des( Node* );
NodeGenerationNeeds* get_needs( Node* n );
}

#endif /* CB_GEN_GEN_SWITCH_H_INCLUDED */
