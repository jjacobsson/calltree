/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef PARALLEL_NODE_GRIST_H_
#define PARALLEL_NODE_GRIST_H_

#include "node_grist.h"


class NodeList;
class CReturnState;

class ParallelNodeGrist : public NodeGrist
{
public:
    ParallelNodeGrist(  );
    ~ParallelNodeGrist();

    int GetType() const { return E_GRIST_PARALLEL; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void Visit( INodeVisitor* nv );
    void SetChaff( Node* chaff );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:
    NodeList* m_Children;
    int       m_bss_SuccessCounter;
};



#endif /*PARALLEL_NODE_GRIST_H_*/
