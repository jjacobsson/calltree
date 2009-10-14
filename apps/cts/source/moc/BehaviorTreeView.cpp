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
	qreal factor = 1.2;
	if (event->delta() < 0 )
		factor = 1.0 / factor;
	scale( factor, factor );
}
