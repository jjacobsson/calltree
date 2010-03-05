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

BehaviorTreeSceneItem::BehaviorTreeSceneItem( QGraphicsObject* parent )
  : QGraphicsObject( parent )
  , m_MouseState( E_MS_NONE )
  , m_PropertyWidget( 0x0 )
{
  setFlag( QGraphicsItem::ItemIsMovable, true );
  setFlag( QGraphicsItem::ItemIsSelectable, true );
  setFlag( QGraphicsItem::ItemStacksBehindParent, false );

  setZValue( 0.0 );
}

BehaviorTreeSceneItem::~BehaviorTreeSceneItem()
{
  removeArrows();
  delete m_PropertyWidget;
  m_PropertyWidget = 0x0;
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

void BehaviorTreeSceneItem::dragMove()
{
}

void BehaviorTreeSceneItem::dragBegin()
{
}

void BehaviorTreeSceneItem::dragEnd()
{
  emit itemDragged();
}

void BehaviorTreeSceneItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
  if(!(option->state & QStyle::State_Selected))
    return;

  QRectF brect( boundingRect() );
  const QRectF mbrect = painter->transform().mapRect(brect);
  if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
      return;

  qreal itemPenWidth = 1;
  qreal penWidth = 0;
  const qreal pad = itemPenWidth / 2;
  const QColor fgcolor = option->palette.windowText().color();
  const QColor bgcolor( // ensure good contrast against fgcolor
      fgcolor.red()   > 127 ? 0 : 255,
      fgcolor.green() > 127 ? 0 : 255,
      fgcolor.blue()  > 127 ? 0 : 255);

  painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(brect.adjusted(pad, pad, -pad, -pad));

  painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(brect.adjusted(pad, pad, -pad, -pad));
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
    if( value.toBool() )
      emit itemSelected( m_PropertyWidget );
    else
      emit itemSelected( 0x0 );
    break;
  default:
    break;
  }
  return QGraphicsItem::itemChange( change, value );
}

void BehaviorTreeSceneItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
    m_MouseState = E_MS_LB_DOWN;
    m_StartPos = event->screenPos();
  }
  QGraphicsItem::mousePressEvent( event );
}

void BehaviorTreeSceneItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
    if( m_MouseState == E_MS_DRAGGING )
      dragEnd();

    m_MouseState = E_MS_NONE;
  }
  QGraphicsItem::mouseReleaseEvent( event );
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

  QGraphicsItem::mouseMoveEvent( event );
}

void BehaviorTreeSceneItem::signalModified()
{
  emit modified();
}
