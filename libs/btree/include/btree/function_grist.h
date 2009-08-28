/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef FUNCTION_GRIST_H_
#define FUNCTION_GRIST_H_

#include "variable.h"
#include <vector>

class BehaviourTree;

class FunctionGrist
{
public:
    FunctionGrist();
    ~FunctionGrist();

    bool ValiadateVariables( Variable* variables ) const;

    int            m_bssSize;
    Variable*      m_Variables;
    BehaviourTree* m_Tree;
    bool           m_Construct;
    bool           m_Destruct;
};

#endif /*FUNCTION_GRIST_H_*/
