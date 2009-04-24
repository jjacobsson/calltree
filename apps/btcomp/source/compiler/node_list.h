/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef CHILD_LIST_H_
#define CHILD_LIST_H_

class Node;

class NodeList
{
public:

	NodeList()
		: m_Count( 0 )
	{}
	
	static const int s_MaxNodeCount = 128;
	
	typedef Node**					iterator;
	typedef const Node* const *	const_iterator;

	inline iterator begin()							{ return &m_Nodes[0]; }
	inline iterator end()							{ return &m_Nodes[m_Count]; }
	
	inline const_iterator begin() const				{ return &m_Nodes[0]; }
	inline const_iterator end() const				{ return &m_Nodes[m_Count]; }
	
	inline Node* operator[] (int i)				{ return m_Nodes[i]; }
	inline const Node* operator[] (int i) const	{ return m_Nodes[i]; }
	
	void SetParent( Node* node );
	bool Append( Node* node );
	int Size();
	
private:
	Node*	m_Nodes[ s_MaxNodeCount ];
	int		m_Count;
};

#endif /*CHILD_LIST_H_*/
