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

#include "BehaviorTreeSceneItem.h"
#include "../NodeToNodeArrow.h"
#include "../standard_resources.h"
#include <btree/btree.h>

#include <QtGui/QtGui>
#include <QtSvg/QSvgRenderer>

BehaviorTreeSceneItem::BehaviorTreeSceneItem( const char* gfx_path, BehaviorTreeSceneItem* parent )
  : QGraphicsSvgItem( gfx_path, parent )
  , m_MouseState( E_MS_NONE )
{
  setFlag( QGraphicsItem::ItemIsMovable, true );
  setFlag( QGraphicsItem::ItemIsSelectable, true );
  setFlag( QGraphicsItem::ItemStacksBehindParent, false );

  setZValue( 0.0 );
}

void BehaviorTreeSceneItem::removeArrow(NodeToNodeArrow *arrow)
{
    int index = m_Arrows.indexOf(arrow);

    if (index != -1)
        m_Arrows.removeAt(index);
}

void BehaviorTreeSceneItem::removeArrows()
{
  foreach( NodeToNodeArrow *arrow, m_Arrows )
  {
    arrow->startItem()->removeArrow(arrow);
    arrow->endItem()->removeArrow(arrow);
    scene()->removeItem(arrow);
    delete arrow;
  }
}

void BehaviorTreeSceneItem::addArrow(NodeToNodeArrow *arrow)
{
  m_Arrows.append(arrow);
}

NodeToNodeArrow* BehaviorTreeSceneItem::findArrowTo( BehaviorTreeSceneItem* other )
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

QVariant BehaviorTreeSceneItem::itemChange( GraphicsItemChange change,
  const QVariant &value )
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

void BehaviorTreeSceneItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
    m_MouseState = E_MS_LB_DOWN;
    m_StartPos = event->screenPos();
  }
  QGraphicsSvgItem::mousePressEvent( event );
}

void BehaviorTreeSceneItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
    if( m_MouseState == E_MS_DRAGGING )
      dragEnd();

    m_MouseState = E_MS_NONE;
  }
  QGraphicsSvgItem::mouseReleaseEvent( event );
}

void BehaviorTreeSceneItem::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
  if( m_MouseState == E_MS_LB_DOWN )
  {
    if( m_StartPos != event->screenPos() )
    {
      dragBegin();
      m_MouseState = E_MS_DRAGGING;
    }
  }
  if( m_MouseState == E_MS_DRAGGING )
    dragMove();

  QGraphicsSvgItem::mouseMoveEvent( event );
}

void BehaviorTreeSceneItem::dragMove()
{
}

void BehaviorTreeSceneItem::dragBegin()
{
  setZValue( 1.0 );
}

void BehaviorTreeSceneItem::dragEnd()
{
  setZValue( 0.0 );
  emit itemDragged();
}

