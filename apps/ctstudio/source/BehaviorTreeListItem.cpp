/*******************************************************************************
 * Copyright (c) 2010-03-09 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "BehaviorTreeListItem.h"

BehaviorTreeListItem::BehaviorTreeListItem( int sort_key ) :
  QListWidgetItem( 0x0, UserType ), m_SortKey( sort_key )
{
}

BehaviorTreeListItem::BehaviorTreeListItem( const BehaviorTreeListItem& other ) :
  QListWidgetItem( other ), m_SortKey( other.m_SortKey )
{
}

QListWidgetItem* BehaviorTreeListItem::clone() const
{
  return new BehaviorTreeListItem( *this );
}

bool BehaviorTreeListItem::operator <( const QListWidgetItem& other ) const
{
  const BehaviorTreeListItem* o = (const BehaviorTreeListItem*)(&other);

  if( m_SortKey == o->m_SortKey )
    return text() < o->text();
  return m_SortKey < o->m_SortKey;
}
