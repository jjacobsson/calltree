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

#ifndef NODEPROPERTYDELEGATE_H_INCLUDED
#define NODEPROPERTYDELEGATE_H_INCLUDED

#include <QtGui/QStyledItemDelegate>
#include <QtCore/QModelIndex>
#include <QtCore/QObject>
#include <QtCore/QSize>

class NodePropertyDelegate: public QStyledItemDelegate
{
Q_OBJECT
public:

  NodePropertyDelegate( QObject* parent = 0x0 );

  QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index ) const;

  void setEditorData( QWidget *editor, const QModelIndex &index ) const;
  void setModelData( QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index ) const;

  void updateEditorGeometry( QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index ) const;

};

#endif /* NODEPROPERTYDELEGATE_H_INCLUDED */
