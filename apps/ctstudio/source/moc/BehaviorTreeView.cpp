/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "BehaviorTreeView.h"
#include "BehaviorTreeScene.h"

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

BehaviorTreeView::BehaviorTreeView()
	: QGraphicsView()
{
	QGLFormat format( QGL::SampleBuffers );
	QGLWidget* gl_widget = new QGLWidget( format );

	if( gl_widget->isValid() )
	{
		this->setViewport( gl_widget );
		this->setRenderHint(QPainter::Antialiasing, true);
	}
	else
		delete gl_widget;
}

BehaviorTreeView::BehaviorTreeView( QGraphicsScene* scene )
	: QGraphicsView( scene )
{
	QGLFormat format( QGL::SampleBuffers );
	QGLWidget* gl_widget = new QGLWidget( format );

	if( gl_widget->isValid() )
	{
		this->setViewport( gl_widget );
		this->setRenderHint(QPainter::Antialiasing, true);
	}
	else
		delete gl_widget;
}

void BehaviorTreeView::wheelEvent( QWheelEvent* event )
{
	QPoint mouse_pos( event->pos() );
	QPointF scene_pos( mapToScene( mouse_pos ) );

	qreal factor = 1.1;
	if (event->delta() < 0 )
		factor = 1.0 / factor;
	scale( factor, factor );

	QPoint view_pos( mapFromScene( scene_pos ) );

	QPoint delta( mouse_pos - view_pos );

    QScrollBar *hBar = horizontalScrollBar();
    QScrollBar *vBar = verticalScrollBar();
    hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
    vBar->setValue(vBar->value() - delta.y());

	event->accept();
}

void BehaviorTreeView::mousePressEvent( QMouseEvent* e )
{
	QGraphicsView::mousePressEvent( e );
}

void BehaviorTreeView::mouseReleaseEvent( QMouseEvent* e )
{
	QGraphicsView::mouseReleaseEvent( e );
}

void BehaviorTreeView::keyPressEvent( QKeyEvent* e )
{
	if( e->key() == Qt::Key_Alt )
	{
		setInteractive( false );
		setDragMode( QGraphicsView::ScrollHandDrag );
	}
	QGraphicsView::keyPressEvent( e );
}

void BehaviorTreeView::keyReleaseEvent( QKeyEvent* e )
{
	if( e->key() == Qt::Key_Alt )
	{
		setInteractive( true );
		setDragMode( QGraphicsView::NoDrag );
	}
	QGraphicsView::keyReleaseEvent( e );
}

void BehaviorTreeView::dragEnterEvent( QDragEnterEvent *event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*>(scene());
  if( s )
    s->dragEnterEvent( event );
  else
    event->ignore();
}

void BehaviorTreeView::dragLeaveEvent( QDragLeaveEvent *event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*>(scene());
  if( s )
    s->dragLeaveEvent( event );
  else
    event->ignore();
}

void BehaviorTreeView::dragMoveEvent( QDragMoveEvent *event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*>(scene());
  if( s )
    s->dragMoveEvent( event );
  else
    event->ignore();
}

void BehaviorTreeView::dropEvent( QDropEvent* event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*>(scene());
  if( s )
    s->dropEvent( event );
  else
    event->ignore();
}
