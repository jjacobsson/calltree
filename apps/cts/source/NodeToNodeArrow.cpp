/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "NodeToNodeArrow.h"
#include "moc/BehaviorTreeNode.h"

#include <QtGui/QtGui>

NodeToNodeArrow::NodeToNodeArrow(
		BehaviorTreeNode* start,
		BehaviorTreeNode* end,
		QGraphicsScene* scene
	)
	: QGraphicsLineItem( 0x0, scene)
	, m_Start( start )
	, m_End( end )
{
	setFlag(QGraphicsItem::ItemIsSelectable, false );
	setFlag(QGraphicsItem::ItemStacksBehindParent, true );
	setPen(QPen(Qt::black, 6.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	setZValue( -1000.0 );
}

 QRectF NodeToNodeArrow::boundingRect() const
 {
	 qreal extra = (pen().width() + 20) / 2.0;

	 return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
									   line().p2().y() - line().p1().y()))
		 .normalized()
		 .adjusted(-extra, -extra, extra, extra);
 }


void NodeToNodeArrow::updatePosition()
{
	QPointF s,e;
	QRectF sr( m_Start->boundingRect() );
	QRectF er( m_End->boundingRect() );
	s = mapFromItem( m_Start, sr.width() / 2.0 , sr.height() / 2.0 );
	e = mapFromItem( m_End, er.width() / 2.0 , er.height() / 2.0 );
	setLine( QLineF( s, e ) );
}

void NodeToNodeArrow::paint(
		QPainter *painter,
		const QStyleOptionGraphicsItem *,
		QWidget *
	)
{
	if (m_Start->collidesWithItem(m_End))
		return;
	updatePosition();
	painter->setPen( pen() );
	painter->drawLine( line() );
}

