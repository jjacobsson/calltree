/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef BEHAVOIRTREEVIEW_H_INCLUDED
#define BEHAVOIRTREEVIEW_H_INCLUDED

#include <QtGui/QGraphicsView>

#include "../GraphicsItemTypes.h"

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
	void keyPressEvent( QKeyEvent* e );
	void keyReleaseEvent( QKeyEvent* e );

    void dragEnterEvent( QDragEnterEvent *event );
    void dragLeaveEvent( QDragLeaveEvent *event );
    void dragMoveEvent( QDragMoveEvent *event );
    void dropEvent( QDropEvent *event );

};

#endif
