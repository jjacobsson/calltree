/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef XGML_PRINTER_H_
#define XGML_PRINTER_H_

#include "compiler/visitor.h"

#include <vector>
#include <stdlib.h>

class XGMLPrinter : public INodeVisitor
{
public:
	XGMLPrinter();
	~XGMLPrinter();

	void Visit( Node* n );

	void Layout();

	void Print( FILE* file ) const;

private:

	int FindParentIndex( const Node* n ) const;

	struct SSpring
	{
		int m_From;
		int m_To;
	};

	struct SVector
	{
		double x;
		double y;
	};


	struct SNode
	{
		SVector		m_Pos;
		Node* 		m_Node;
		SNode*		m_Parent;
		SNode*		m_PrevDepth;
		SNode*		m_NextDepth;
		int			m_Depth;
		int			m_ChildCount;
		int			m_DepthIndex;		/* index on this depth */
		int			m_ParentIndex;		/* index under parent */
	};

	struct SDepth
	{
		SNode*	m_Last;
		int		m_Count;
	};

	void PrintSequence( FILE* f, const SNode* n ) const;
	void PrintSelector( FILE* f, const SNode* n ) const;
	void PrintParallel( FILE* f, const SNode* n ) const;
	void PrintDynSelector( FILE* f, const SNode* n ) const;
	void PrintDecorator( FILE* f, const SNode* n ) const;
	void PrintAction( FILE* f, const SNode* n ) const;

	void PrintCommonGraphics( FILE* f, const SNode* n ) const;
	void PrintCommonLabel( FILE* f ) const;

	typedef std::vector<SSpring>	CSpringList;
	typedef std::vector<SDepth>		CDepthList;
	typedef std::vector<SNode*>		NodeList;

	CSpringList	m_Springs;
	NodeList	m_Nodes;
	CDepthList	m_Depth;
};



#endif /* XGML_PRINTER_H_ */
