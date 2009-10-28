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
#include <QtSvg/QSvgRenderer>

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
	, m_MouseState( E_MS_NONE )
	, m_DraggingArrow( 0x0 )
{
	setFlag( QGraphicsItem::ItemIsMovable, true );
	setFlag( QGraphicsItem::ItemIsSelectable, true );
	setFlag( QGraphicsItem::ItemStacksBehindParent, false );

	if( parent )
		setParentItem( parent );
	setZValue( 0.0 );
	QGraphicsTextItem* gti = new QGraphicsTextItem( m_Node->m_Id.m_Text, this );

	QPointF p;
	QRectF r( gti->boundingRect() );
	p.rx() = 128.0 - (r.width() / 2.0);
	p.ry() = 128.0 - (r.height() / 2.0);
	gti->setPos( p );
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
	}
	QGraphicsSvgItem::mousePressEvent( event );
}

void BehaviorTreeNode::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
	if( event->button() == Qt::LeftButton )
	{
		if( m_MouseState == E_MS_DRAGGING )
			draggingEnded();

		m_MouseState = E_MS_NONE;
	}
	QGraphicsSvgItem::mouseReleaseEvent( event );
}

void BehaviorTreeNode::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
	if( m_MouseState == E_MS_LB_DOWN )
	{
		if( m_StartPos != event->screenPos() )
		{
			draggingStarted();
			m_MouseState = E_MS_DRAGGING;
		}
	}
	if( m_MouseState == E_MS_DRAGGING )
		lookForRelinkTarget();

	QGraphicsSvgItem::mouseMoveEvent( event );
}

void BehaviorTreeNode::draggingStarted()
{
	if( parentItem() )
	{
		BehaviorTreeNode* p = (BehaviorTreeNode*)parentItem();
		m_DraggingArrow = findArrowTo( p );
		p->removeArrow( m_DraggingArrow );
		m_DraggingArrow->setDashed( true );
		m_DraggingArrow->setStartAndEnd( this, p );

		QPointF position( scenePos() );
		setParentItem( 0x0 );
		setPos( position );
	}
	else
	{
		m_DraggingArrow = new NodeToNodeArrow( this, 0x0, scene() );
		m_DraggingArrow->setDashed( true );
		addArrow( m_DraggingArrow );
	}

	setupRelinkage();

	setZValue( 1.0 );
}

void BehaviorTreeNode::draggingEnded()
{
	if( m_Relinkage.m_Parent )
	{
		BehaviorTreeNode* p = (BehaviorTreeNode*)m_Relinkage.m_Parent->m_UserData;
		setPos( p->mapFromScene( scenePos() ) );
		setParentItem( p );

		p->addArrow( m_DraggingArrow );
		m_DraggingArrow->setStartAndEnd( p, this );
		m_DraggingArrow->setDashed( false );
		m_DraggingArrow = 0x0;
	}
	else
	{
		removeArrow( m_DraggingArrow );
		delete m_DraggingArrow;
		m_DraggingArrow = 0x0;
	}

	executeRelinkage();

	setZValue( 0.0 );

	emit nodeDragged();
}

void BehaviorTreeNode::setupRelinkage()
{
	m_Relinkage.m_Parent = m_Node->m_Pare;
	if( m_Node->m_Prev )
	{
		m_Relinkage.m_Sibling		= m_Node->m_Prev;
		m_Relinkage.m_BeforeSibling	= false;
	}
	else if( m_Node->m_Next )
	{
		m_Relinkage.m_Sibling		= m_Node->m_Next;
		m_Relinkage.m_BeforeSibling	= true;
	}
	else
	{
		m_Relinkage.m_Sibling		= 0x0;
		m_Relinkage.m_BeforeSibling = false;
	}
	UnlinkNodeFromParentAndSiblings( m_Node );
}

void BehaviorTreeNode::executeRelinkage()
{
	if( m_Relinkage.m_Parent )
		m_Node->m_Pare = m_Relinkage.m_Parent;

	if( m_Relinkage.m_Sibling )
	{
		if( m_Relinkage.m_BeforeSibling )
		{
			if( m_Relinkage.m_Sibling->m_Prev )
				m_Relinkage.m_Sibling->m_Prev->m_Next = m_Node;
			else
				SetFirstChild( m_Node->m_Pare, m_Node );

			m_Node->m_Prev = m_Relinkage.m_Sibling->m_Prev;

			m_Relinkage.m_Sibling->m_Prev = m_Node;
			m_Node->m_Next = m_Relinkage.m_Sibling;
		}
		else
		{
			if( m_Relinkage.m_Sibling->m_Next )
				m_Relinkage.m_Sibling->m_Next->m_Prev = m_Node;
			m_Node->m_Next = m_Relinkage.m_Sibling->m_Next;
			m_Relinkage.m_Sibling->m_Next = m_Node;
			m_Node->m_Prev = m_Relinkage.m_Sibling;
		}
	}
	else
	{
		m_Node->m_Next = 0x0;
		m_Node->m_Prev = 0x0;
		SetFirstChild( m_Node->m_Pare, m_Node );
	}
}

void BehaviorTreeNode::lookForRelinkTarget()
{
	QList<QGraphicsItem*> coll( collidingItems() );
	foreach( QGraphicsItem* uknown_item, coll )
	{
		if( uknown_item->type() != Type )
			continue;

		BehaviorTreeNode* item = (BehaviorTreeNode*)uknown_item;
		Node* p = item->m_Node;

		if( !AcceptsMoreChildren( p ) )
			continue;

		// Current parent does not need evaluation.
		if( p == m_Relinkage.m_Parent )
			continue;

		Relinkage t;

		t.m_Parent  		= p;
		t.m_Sibling			= GetFirstChild( p );
		t.m_BeforeSibling 	= true;

		m_DraggingArrow->setStartAndEnd( this, (BehaviorTreeNode*)p->m_UserData );
		m_Relinkage = t;
	}

	BehaviorTreeNode* item;
	Node* n		= GetFirstChild( m_Relinkage.m_Parent );

	if( !n )
		return;

	item		= (BehaviorTreeNode*)n->m_UserData;
	qreal x		= scenePos().x();
	qreal tx	= item->scenePos().x();
	qreal best	= qAbs( x - tx );

	m_Relinkage.m_Sibling = n;
	if( tx < x )
		m_Relinkage.m_BeforeSibling = false;
	else
		m_Relinkage.m_BeforeSibling = true;

	while( n )
	{
		item	= (BehaviorTreeNode*)n->m_UserData;
		tx		= item->scenePos().x();
		qreal d	= qAbs( x - tx );
		if( d < best )
		{
			m_Relinkage.m_Sibling = n;
			if( tx < x )
				m_Relinkage.m_BeforeSibling = false;
			else
				m_Relinkage.m_BeforeSibling = true;
			best = d;
		}
		n = n->m_Next;
	}
}

