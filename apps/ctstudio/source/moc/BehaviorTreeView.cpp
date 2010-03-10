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

#include "BehaviorTreeView.h"
#include "BehaviorTreeScene.h"

#include <QtGui/QtGui>

BehaviorTreeView::BehaviorTreeView() :
  QGraphicsView()
{
  this->setRenderHint( QPainter::Antialiasing, true );
  this->setRenderHint( QPainter::TextAntialiasing, true );
  this->setRenderHint( QPainter::HighQualityAntialiasing, true );
}

BehaviorTreeView::BehaviorTreeView( QGraphicsScene* scene ) :
  QGraphicsView( scene )
{
  this->setRenderHint( QPainter::Antialiasing, true );
  this->setRenderHint( QPainter::TextAntialiasing, true );
  this->setRenderHint( QPainter::HighQualityAntialiasing, true );
}

void BehaviorTreeView::wheelEvent( QWheelEvent* event )
{
  QPoint mouse_pos( event->pos() );
  QPointF scene_pos( mapToScene( mouse_pos ) );

  qreal factor = 1.1;
  if( event->delta() < 0 )
    factor = 1.0 / factor;
  scale( factor, factor );

  QPoint view_pos( mapFromScene( scene_pos ) );

  QPoint delta( mouse_pos - view_pos );

  QScrollBar *hBar = horizontalScrollBar();
  QScrollBar *vBar = verticalScrollBar();
  hBar->setValue( hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()) );
  vBar->setValue( vBar->value() - delta.y() );

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
  switch( e->key() )
  {
  case Qt::Key_Alt:
    setInteractive( false );
    setDragMode( QGraphicsView::ScrollHandDrag );
    break;
  case Qt::Key_Return:
  case Qt::Key_Enter:
    break;
  default:
    QGraphicsView::keyPressEvent( e );
    break;
  }
}

void BehaviorTreeView::keyReleaseEvent( QKeyEvent* e )
{
  switch( e->key() )
  {
  case Qt::Key_Alt:
    setInteractive( true );
    setDragMode( QGraphicsView::NoDrag );
    break;
  default:
    QGraphicsView::keyReleaseEvent( e );
  }
}

void BehaviorTreeView::dragEnterEvent( QDragEnterEvent *event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*> ( scene() );
  if( s )
    s->dragEnterEvent( event );
  else
    event->ignore();
}

void BehaviorTreeView::dragLeaveEvent( QDragLeaveEvent *event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*> ( scene() );
  if( s )
    s->dragLeaveEvent( event );
  else
    event->ignore();
}

void BehaviorTreeView::dragMoveEvent( QDragMoveEvent *event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*> ( scene() );

  if( s )
  {
    QPointF pos( mapToScene( event->pos() ) );
    s->dragMoveEvent( event, pos );
  }
  else
    event->ignore();
}

void BehaviorTreeView::dropEvent( QDropEvent* event )
{
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*> ( scene() );
  if( s )
    s->dropEvent( event );
  else
    event->ignore();
}
