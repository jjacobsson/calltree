/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef VISITOR_H_
#define VISITOR_H_

class Node;

class INodeVisitor
{
public:
	~INodeVisitor() {}

	virtual void Visit( Node* n ) = 0;

};

#endif /* VISITOR_H_ */
