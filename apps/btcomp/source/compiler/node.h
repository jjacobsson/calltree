/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef NODE_H_
#define NODE_H_

class Identifier;
class NodeGrist;
class BehaviourTree;
class INodeVisitor;

class Node
{
public:
	Node()
		: m_Tree( 0x0 )
		, m_Parent( 0x0 )
		, m_Id( 0x0 )
		, m_Grist( 0x0 )
		, m_IsChild( false )
	{}

	~Node();

	void Visit( INodeVisitor* v );

	BehaviourTree*	m_Tree;
	Node*			m_Parent;
	Identifier*	m_Id;
	NodeGrist*		m_Grist;
	bool			m_IsChild;
};


#endif /*NODE_H_*/
