/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef SELECTOR_NODE_GRIST_H_
#define SELECTOR_NODE_GRIST_H_

#include "../node_grist.h"

class NodeList;

class SelectorNodeGrist : public NodeGrist
{
public:
    SelectorNodeGrist();
    ~SelectorNodeGrist();

    int GetType() const { return E_GRIST_SELECTOR; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void Visit( INodeVisitor* nv );
    void SetChaff( Node* n );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:
    NodeList*  m_Children;
    int        m_bss_ReEntry;
    int        m_bss_JumpBackTarget;
};

#endif /*SELECTOR_NODE_GRIST_H_*/
