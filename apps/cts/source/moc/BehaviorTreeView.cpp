/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "BehaviorTreeView.h"

#include <QtGui/QtGui>

BehaviorTreeView::BehaviorTreeView()
	: QGraphicsView()
{

}

BehaviorTreeView::BehaviorTreeView( QGraphicsScene* scene )
	: QGraphicsView( scene )
{

}

void BehaviorTreeView::wheelEvent( QWheelEvent* event )
{
	QPointF p = mapToScene( event->pos() );
	qreal factor = 1.2;
	if (event->delta() < 0 )
		factor = 1.0 / factor;
	centerOn( p );
	scale( factor, factor );


	event->accept();
}

void BehaviorTreeView::mousePressEvent( QMouseEvent* e )
{
	if( e->button() == Qt::MidButton )
	{
	}
	QGraphicsView::mousePressEvent( e );
}

void BehaviorTreeView::mouseReleaseEvent( QMouseEvent* e )
{
	if( e->button() == Qt::MidButton )
	{
	}
	QGraphicsView::mouseReleaseEvent( e );
}
