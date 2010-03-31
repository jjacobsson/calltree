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

#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>

#include <float.h>

NodePropertyDelegate::NodePropertyDelegate( QObject* parent ) :
  QStyledItemDelegate( parent )
{
}

QWidget* NodePropertyDelegate::createEditor( QWidget *parent,
  const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  int type = index.model()->data( index, Qt::UserRole + 1 ).toInt();
  QWidget* edit = 0x0;
  switch( type )
  {
  case E_VART_INTEGER:
    {
      QSpinBox* sb = new QSpinBox( parent );
      sb->setRange( INT_MIN, INT_MAX );
      edit = sb;
    }
    break;
  case E_VART_FLOAT:
    {
      QDoubleSpinBox* sb = new QDoubleSpinBox( parent );
      sb->setRange( -FLT_MAX, FLT_MAX );
      sb->setDecimals( 4 );
      edit = sb;
    }
    break;
  case E_VART_HASH:
  case E_VART_STRING:
    edit = QStyledItemDelegate::createEditor( parent, option, index );
    break;
  }

  return edit;
}

void NodePropertyDelegate::setEditorData( QWidget *editor,
  const QModelIndex &index ) const
{
  int type = index.model()->data( index, Qt::UserRole + 1 ).toInt();
  switch( type )
  {
  case E_VART_INTEGER:
    {
      int value = index.model()->data( index, Qt::UserRole + 2 ).toInt();
      QSpinBox *spinBox = static_cast<QSpinBox*> ( editor );
      spinBox->setValue( value );
    }
    break;
  case E_VART_FLOAT:
    {
      float value = index.model()->data( index, Qt::UserRole + 2 ).toFloat();
      QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*> ( editor );
      spinBox->setValue( value );
    }
    break;
  case E_VART_HASH:
  case E_VART_STRING:
    QStyledItemDelegate::setEditorData( editor, index );
    break;
  }
}

void NodePropertyDelegate::setModelData( QWidget *editor,
  QAbstractItemModel *model, const QModelIndex &index ) const
{
  int type = index.model()->data( index, Qt::UserRole + 1 ).toInt();
  switch( type )
  {
  case E_VART_INTEGER:
    {
      QSpinBox *spinBox = static_cast<QSpinBox*> ( editor );
      spinBox->interpretText();
      int value = spinBox->value();
      model->setData( index, value, Qt::EditRole );
      model->setData( index, value, Qt::UserRole + 2 );
    }
    break;
  case E_VART_FLOAT:
    {
      QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*> ( editor );
      spinBox->interpretText();
      float value = (float)spinBox->value();
      model->setData( index, value, Qt::EditRole );
      model->setData( index, value, Qt::UserRole + 2 );
    }
    break;
  case E_VART_HASH:
  case E_VART_STRING:
    QStyledItemDelegate::setModelData( editor, model, index );
    break;
  }
}
