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

#include <QtGui/QtGui>

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
	, m_PreviousParent( 0x0 )
	, m_MouseState( E_MS_NONE )
{
	setFlag( QGraphicsItem::ItemIsMovable, true );
	setFlag( QGraphicsItem::ItemIsSelectable, true );
	setFlag( QGraphicsItem::ItemStacksBehindParent, false );

	if( parent )
		setParentItem( parent );
	setZValue( 0.0 );
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

NodeToNodeArrow* BehaviorTreeNode::findArrowTo( BehaviorTreeNode* other )
{
	foreach( NodeToNodeArrow *arrow, m_Arrows )
	{
		if( arrow->startItem() == this && arrow->endItem() == other )
			return arrow;
		if( arrow->startItem() == other && arrow->endItem() == this )
			return arrow;
	}
	return 0x0;
}

QVariant BehaviorTreeNode::itemChange( GraphicsItemChange change, const QVariant &value )
{
	switch( change )
	{
	case ItemSelectedChange:
		update();
		break;
	case ItemPositionChange:
		break;
	}
	return QGraphicsSvgItem::itemChange( change, value );
}

void BehaviorTreeNode::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
	if( event->button() == Qt::LeftButton )
	{
		m_MouseState = E_MS_LB_DOWN;
		m_StartPos = event->screenPos();
		if( parentItem() )
		{
			m_PreviousParent = (BehaviorTreeNode*)parentItem();
			QPointF position( scenePos() );
			setParentItem( 0x0 );
			setPos( position );
		}
	}
	QGraphicsSvgItem::mousePressEvent( event );
}

void BehaviorTreeNode::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
	if( event->button() == Qt::LeftButton )
	{
		if( m_MouseState == E_MS_DRAGGING )
		{
			if( m_PreviousParent )
			{
				NodeToNodeArrow* arrow = findArrowTo( m_PreviousParent );
				if( arrow )
				{
					removeArrow( arrow );
					m_PreviousParent->removeArrow( arrow );
					scene()->removeItem( arrow );
					delete arrow;
				}
			}

			m_PreviousParent = 0x0;

			Node* p = m_Node->m_Pare;
			while( p && p->m_Pare )
				p = p->m_Pare;

			if( p )
			{
				UnlinkNodeFromParentAndSiblings( m_Node );
				AppendToEndOfList( p, m_Node );
			}

			emit nodeDragged();
		}

		m_MouseState = E_MS_NONE;
		setZValue( 0.0 );

		if( m_PreviousParent )
		{
			setPos( m_PreviousParent->mapFromScene( scenePos() ) );
			setParentItem( m_PreviousParent );
			m_PreviousParent = 0x0;
		}
	}
	QGraphicsSvgItem::mouseReleaseEvent( event );
}

void BehaviorTreeNode::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
	if( m_MouseState == E_MS_LB_DOWN )
	{
		if( m_StartPos != event->screenPos() )
		{
			m_MouseState = E_MS_DRAGGING;
			setZValue( 1.0 );
		}
	}
	QGraphicsSvgItem::mouseMoveEvent( event );
}
