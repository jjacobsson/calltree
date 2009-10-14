/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BEHAVOIRTREEVIEW_H_INCLUDED
#define BEHAVOIRTREEVIEW_H_INCLUDED

#include <QtGui/QGraphicsView>

// Qt Forward declares
class QGraphicsScene;

class BehaviorTreeView : public QGraphicsView
{
	Q_OBJECT

public:

	BehaviorTreeView();
	BehaviorTreeView( QGraphicsScene* scene );

protected:
	void wheelEvent( QWheelEvent* e );
	void mousePressEvent( QMouseEvent* e );
	void mouseReleaseEvent( QMouseEvent* e );
};

#endif
