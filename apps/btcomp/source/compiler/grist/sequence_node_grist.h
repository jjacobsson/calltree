/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef SEQUENCE_NODE_GRIST_H_
#define SEQUENCE_NODE_GRIST_H_

#include "../node_grist.h"

class NodeList;

class SequenceNodeGrist : public NodeGrist
{
public:
    SequenceNodeGrist();
    ~SequenceNodeGrist();

    int GetType() const { return E_GRIST_SEQUENCE; }

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

#endif /*SEQUENCE_NODE_GRIST_H_*/
