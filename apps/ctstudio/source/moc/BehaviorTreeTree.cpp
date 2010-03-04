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

#include "BehaviorTreeTree.h"
#include "../standard_resources.h"
#include <btree/btree.h>

#include <QtSvg/QGraphicsSvgItem>

BehaviorTreeTree::BehaviorTreeTree( BehaviorTree* tree )
  : BehaviorTreeSceneItem()
  , m_Tree( tree )
{
  setFlag( QGraphicsItem::ItemIsMovable, false );
  m_Graphics = new QGraphicsSvgItem( ":/nodes/tree.svg", this );
}

void BehaviorTreeTree::destroyResources( BehaviorTreeContext ctx )
{
  BehaviorTreeContextRemoveSymbol( ctx, m_Tree->m_Id.m_Hash );
  BehaviorTreeContextFreeObject( ctx, m_Tree );
  m_Tree = 0x0;
}

BehaviorTreeSceneItem* BehaviorTreeTree::firstChild()
{
  if( m_Tree->m_Root )
    return (BehaviorTreeSceneItem*)m_Tree->m_Root->m_UserData;
  return 0x0;
}

QRectF BehaviorTreeTree::boundingRect() const
{
  return m_Graphics->boundingRect();
}

