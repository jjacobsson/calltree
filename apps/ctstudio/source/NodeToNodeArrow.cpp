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

const QPen g_RegularPen( QPen( Qt::darkGray, 16.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
const QPen g_DashedPen( QPen( Qt::darkGray, 16.0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin ) );

NodeToNodeArrow::NodeToNodeArrow(
    BehaviorTreeSceneItem* start,
    BehaviorTreeSceneItem* end,
    QGraphicsScene* scene
  )
  : QGraphicsLineItem( 0x0 )
  , m_Start( start )
  , m_End( end )
{
  scene->addItem( this );
  setFlag( QGraphicsItem::ItemIsSelectable, false );
  setFlag( QGraphicsItem::ItemStacksBehindParent, true );
  setPen( g_RegularPen );
  setZValue( -1000.0 );
}

void NodeToNodeArrow::setStartAndEnd(
  BehaviorTreeSceneItem *start,
  BehaviorTreeSceneItem *end )
{
  m_Start = start;
  m_End = end;
  updatePosition();
}

void NodeToNodeArrow::setDashed( bool dashed )
{
  if( dashed )
    setPen( g_DashedPen );
  else
    setPen( g_RegularPen );
}

void NodeToNodeArrow::updatePosition()
{
  if( !m_Start || !m_End )
    return;

  if( m_Start->collidesWithItem( m_End ) )
  {
    setVisible( false );
  }
  else
  {
    setVisible( true );
    prepareGeometryChange();
    setLine( QLineF( m_Start->arrowAnchor(), m_End->arrowAnchor() ) );
  }
}

