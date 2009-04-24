/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef DECORATOR_GRIST_H_
#define DECORATOR_GRIST_H_

#include "../variable_owner_helper.h"

class Decorator;

class DecoratorNodeGrist : public VariableOwner
{
public:
    DecoratorNodeGrist();
    ~DecoratorNodeGrist();

    int GetType() const { return E_GRIST_DECORATOR; }

    void GenerateConstructionCode( BehaviourTree* bt );
    void GenerateExecutionCode( BehaviourTree* bt );
    void GenerateDestructionCode( BehaviourTree* bt );
    void Visit( INodeVisitor* nv );
    void SetChaff( Node* n );

    void SetDecorator( Decorator* dec );
    Decorator* GetDecorator();

    void SetChild( Node* child );
    Node* GetChild();

private:
    Decorator* m_Decorator;
    Node*      m_Child;
    int        m_bssPos;
    int        m_bssModPos;
};

#endif /*DECORATOR_GRIST_H_*/
