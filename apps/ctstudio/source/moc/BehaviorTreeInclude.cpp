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

BehaviorTreeInclude::BehaviorTreeInclude( Include* i )
  : BehaviorTreeSceneItem()
  , m_Include( i )
  , m_Graphics( 0x0 )
{
  setFlag( QGraphicsItem::ItemIsMovable, false );
  m_Graphics = new QGraphicsSvgItem( ":/nodes/include.svg", this );
}

void BehaviorTreeInclude::destroyResources( BehaviorTreeContext ctx )
{
  BehaviorTreeContextReleaseInclude( ctx, m_Include );
  m_Include = 0x0;
}

QRectF BehaviorTreeInclude::boundingRect() const
{
  return m_Graphics->boundingRect();
}






