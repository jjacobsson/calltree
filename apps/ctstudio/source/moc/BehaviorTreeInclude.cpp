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

#include "BehaviorTreeInclude.h"
#include "../standard_resources.h"
#include <btree/btree.h>

#include <QtSvg/QGraphicsSvgItem>
#include <QtGui/QFont>
#include <QtCore/QFileInfo>

BehaviorTreeInclude::BehaviorTreeInclude( Include* i )
  : BehaviorTreeSceneItem()
  , m_Include( i )
  , m_Graphics( 0x0 )
  , m_Label( 0x0 )
{
  setFlag( QGraphicsItem::ItemIsMovable, false );
  m_Graphics = new QGraphicsSvgItem( ":/nodes/include.svg", this );
  setupLabel();
}

void BehaviorTreeInclude::destroyResources( BehaviorTreeContext ctx )
{
  release_include( ctx, m_Include );
  m_Include = 0x0;
}

QRectF BehaviorTreeInclude::boundingRect() const
{
  QRectF rect = m_Graphics->boundingRect().translated( m_Graphics->pos() );
  if( m_Label )
    rect |= m_Label->boundingRect().translated( m_Label->pos() );
  return rect;
}

void BehaviorTreeInclude::setupLabel()
{
  prepareGeometryChange();

  delete m_Label;
  m_Label = 0x0;

  m_Graphics->setPos( 0, 0 );

  QFileInfo fi( m_Include->m_Name );

  QFont font;
  font.setStyleHint( QFont::Helvetica, QFont::PreferAntialias );
  font.setPixelSize(64);
  m_Label = new QGraphicsTextItem( fi.fileName(), this );
  m_Label->setFont( font );
  QPointF p;
  QRectF r( m_Label->boundingRect() );

  p.rx() = 0.0;

  float move_node = (r.width() - 256.0) / 2.0;
  if( move_node > 0 )
    m_Graphics->setPos( move_node, 0.0 );
  else
    p.rx() = -move_node;

  p.ry() = -r.height();
  m_Label->setPos( p );
}






