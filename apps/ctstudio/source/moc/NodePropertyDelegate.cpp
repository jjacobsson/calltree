/*******************************************************************************
 * Copyright (c) 2010-03-31 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "NodePropertyDelegate.h"
#include <btree/btree_data.h>

NodePropertyDelegate::NodePropertyDelegate( QObject* parent ) :
  QItemDelegate( parent )
{

}

QWidget* NodePropertyDelegate::createEditor( QWidget *parent,
  const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  int type = index.model()->data( index, Qt::UserRole ).toInt();

  QWidget* edit = 0x0;

  switch( type )
  {
  case E_VART_BOOL:
    edit = QItemDelegate::createEditor( parent, option, index );
    break;
  }

  return edit;
}

void NodePropertyDelegate::setEditorData( QWidget *editor,
  const QModelIndex &index ) const
{

}

void NodePropertyDelegate::setModelData( QWidget *editor,
  QAbstractItemModel *model, const QModelIndex &index ) const
{

}

void NodePropertyDelegate::updateEditorGeometry( QWidget *editor,
  const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

}
