/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef NODE_GRIST_H_
#define NODE_GRIST_H_

class BehaviourTree;
class Node;
class INodeVisitor;

enum GristTypes
{
	E_GRIST_SEQUENCE,
	E_GRIST_SELECTOR,
	E_GRIST_PARALLEL,
	E_GRIST_DYN_SELECTOR,
	E_GRIST_DECORATOR,
	E_GRIST_ACTION,

	_E_MAX_GRIST_TYPES_
};


class NodeGrist
{
public:
	NodeGrist();
	virtual ~NodeGrist();

	virtual int GetType() const = 0;

	virtual void GenerateConstructionCode( BehaviourTree* bt ) {};
	virtual void GenerateExecutionCode( BehaviourTree* bt ) {};
	virtual void GenerateDestructionCode( BehaviourTree* bt ) {};
	virtual void Visit( INodeVisitor* nv ) {};

	virtual void SetChaff( Node* chaff )
	{
		m_Chaff = chaff;
	}

	static int s_GristCount;

	Node* m_Chaff;
};

#endif /*NODE_GRIST_H_*/
