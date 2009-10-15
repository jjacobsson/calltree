/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "BehaviorTreeNode.h"
#include "../NodeToNodeArrow.h"
#include <btree/btree.h>

#include <QtGui/QGraphicsScene>

const char* const g_NodeResourcePaths[_E_MAX_GRIST_TYPES_] = {
	":/nodes/unknown.svg",
	":/nodes/sequence.svg",
	":/nodes/selector.svg",
	":/nodes/parallel.svg",
	":/nodes/dyn_selector.svg",
	":/nodes/decorator.svg",
	":/nodes/action.svg"
};

BehaviorTreeNode::BehaviorTreeNode( Node* n, BehaviorTreeNode* parent )
	: QGraphicsSvgItem( g_NodeResourcePaths[n->m_Grist.m_Type] )
	, m_Node( n )
{
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

	if( parent )
		setParentItem( parent );
}

void BehaviorTreeNode::removeArrow(NodeToNodeArrow *arrow)
{
	int index = m_Arrows.indexOf(arrow);

	if (index != -1)
		m_Arrows.removeAt(index);
}

void BehaviorTreeNode::removeArrows()
{
	foreach( NodeToNodeArrow *arrow, m_Arrows )
	{
		arrow->startItem()->removeArrow(arrow);
		arrow->endItem()->removeArrow(arrow);
		scene()->removeItem(arrow);
		delete arrow;
	}
}

void BehaviorTreeNode::addArrow(NodeToNodeArrow *arrow)
{
	m_Arrows.append(arrow);
}

QVariant BehaviorTreeNode::itemChange( GraphicsItemChange change, const QVariant &value )
{
	switch( change )
	{
	case ItemSelectedChange:
		update();
		break;
	case ItemPositionChange:
		foreach (NodeToNodeArrow *arrow, m_Arrows)
		{
			arrow->updatePosition();
		}
		break;
	}
	return value;
}
