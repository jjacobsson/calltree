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

#include "NodeToNodeArrow.h"
#include "moc/BehaviorTreeSceneItem.h"

#include <QtGui/QtGui>

NodeToNodeArrow::NodeToNodeArrow(
    BehaviorTreeSceneItem* start,
    BehaviorTreeSceneItem* end,
    QGraphicsScene* scene
  )
  : QGraphicsLineItem( start, scene )
  , m_Start( start )
  , m_End( end )
{
  setFlag( QGraphicsItem::ItemIsSelectable, false );
  setFlag( QGraphicsItem::ItemStacksBehindParent, true );
  setPen( QPen( Qt::black, 16.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  setZValue( -1000.0 );
}

void NodeToNodeArrow::setStartAndEnd(
  BehaviorTreeSceneItem *start,
  BehaviorTreeSceneItem *end )
{
  m_Start = start;
  m_End = end;

  if( m_Start )
    setParentItem( m_Start );
  else
    setParentItem( 0x0 );

  update();
}

void NodeToNodeArrow::setDashed( bool dashed )
{
  if( dashed )
    setPen( QPen( Qt::black, 16.0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin ) );
  else
    setPen( QPen( Qt::black, 16.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
}

void NodeToNodeArrow::updatePosition()
{
  if( !m_Start || !m_End )
    return;

  QPointF s, e;
  QRectF sr( m_Start->boundingRect() );
  QRectF er( m_End->boundingRect() );
  s = mapFromItem( m_Start, sr.width() / 2.0, sr.height() / 2.0 );
  e = mapFromItem( m_End, er.width() / 2.0, er.height() / 2.0 );
  setLine( QLineF( s, e ) );
}

void NodeToNodeArrow::paint( QPainter *painter,
  const QStyleOptionGraphicsItem *, QWidget * )
{
  if( !m_Start || !m_End )
    return;

  if( m_Start->collidesWithItem( m_End ) )
    return;
  updatePosition();
  painter->setPen( pen() );
  painter->drawLine( line() );
}

