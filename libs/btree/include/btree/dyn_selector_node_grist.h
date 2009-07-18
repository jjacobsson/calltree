/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef DYN_SELECTOR_NODE_GRIST_H_
#define DYN_SELECTOR_NODE_GRIST_H_

#include "node_grist.h"
#include <vector>

class NodeList;

class DynamicSelectorNodeGrist : public NodeGrist
{
public:
    DynamicSelectorNodeGrist();
    ~DynamicSelectorNodeGrist();

    int GetType() const { return E_GRIST_DYN_SELECTOR; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void Visit( INodeVisitor* nv );
    void SetChaff( Node* n );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:
    typedef std::vector<int> PatchList;

    NodeList*  m_Children;
    int        m_bss_NewBranch;
    int        m_bss_OldBranch;
    int        m_bss_JumpBackTarget;
    int        m_bss_RunningChild;
    PatchList m_DestructionCodeLocation;
};

#endif /*SELECTOR_NODE_GRIST_H_*/
