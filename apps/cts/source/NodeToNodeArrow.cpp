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
		QGraphicsItem* parent,
		QGraphicsScene* scene
	)
	: QGraphicsLineItem(parent, scene)
	, m_Start( start )
	, m_End( end )
{
	setFlag(QGraphicsItem::ItemIsSelectable, false );
	setPen(QPen(Qt::black, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void NodeToNodeArrow::updatePosition()
{
	QLineF line(mapFromItem(m_Start, 128.0, 128.0), mapFromItem(m_End, 128.0, 128.0));
	setLine(line);
}

/*
void NodeToNodeArrow::paint(
		QPainter *painter,
		const QStyleOptionGraphicsItem *,
		QWidget *
	)
{

	if (myStartItem->collidesWithItem(myEndItem))
		return;

	QPen myPen = pen();
	myPen.setColor(myColor);
	qreal arrowSize = 20;
	painter->setPen(myPen);
	painter->setBrush(myColor);

	QLineF centerLine(myStartItem->pos(), myEndItem->pos());
	QPolygonF endPolygon = myEndItem->polygon();
	QPointF p1 = endPolygon.first() + myEndItem->pos();
	QPointF p2;
	QPointF intersectPoint;
	QLineF polyLine;
	for (int i = 1; i < endPolygon.count(); ++i)
	{
		p2 = endPolygon.at(i) + myEndItem->pos();
		polyLine = QLineF(p1, p2);
		QLineF::IntersectType intersectType =
		polyLine.intersect(centerLine, &intersectPoint);
		if (intersectType == QLineF::BoundedIntersection)
			break;
		p1 = p2;
	}

	setLine(QLineF(intersectPoint, myStartItem->pos()));

	double angle = ::acos(line().dx() / line().length());
	if (line().dy() >= 0)
	angle = (Pi * 2) - angle;

	QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
	cos(angle + Pi / 3) * arrowSize);
	QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
	cos(angle + Pi - Pi / 3) * arrowSize);

	arrowHead.clear();
	arrowHead << line().p1() << arrowP1 << arrowP2;
	painter->drawLine(line());
	painter->drawPolygon(arrowHead);

}
*/
