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

BehaviorTreeInclude::BehaviorTreeInclude( Include* i )
  : BehaviorTreeSceneItem( ":/nodes/include.svg" )
  , m_Include( i )
{
  setFlag( QGraphicsItem::ItemIsMovable, false );
}

void BehaviorTreeInclude::destroyResources( BehaviorTreeContext ctx )
{
  BehaviorTreeContextReleaseInclude( ctx, m_Include );
  m_Include = 0x0;
}
