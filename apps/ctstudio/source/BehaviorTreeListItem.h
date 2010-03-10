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

#ifndef BEHAVIORTREELISTITEM_H_INCLUDE
#define BEHAVIORTREELISTITEM_H_INCLUDE

#include <QtGui/QListWidgetItem>

class BehaviorTreeListItem: public QListWidgetItem
{
public:

  BehaviorTreeListItem( int sort_key );
  BehaviorTreeListItem( const BehaviorTreeListItem& o );

  QListWidgetItem* clone() const;

  bool operator < ( const QListWidgetItem& o ) const;

private:

  const int m_SortKey;

};
#endif /* BEHAVIORTREELISTITEM_H_INCLUDE */
