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

#include <QtGui/QtGui>

#include "BehaviorTreeList.h"

#include "../standard_resources.h"

BehaviorTreeList::BehaviorTreeList( QWidget *parent ) :
  QListWidget( parent )
{
  setDragEnabled( true );
  setViewMode( QListView::ListMode );
  setIconSize( QSize( 16, 16 ) );
  setSpacing( 2 );
  setAcceptDrops( false );
  setDropIndicatorShown( false );

  for( int i = 1; i < E_GRIST_DECORATOR; ++i )
  {
    QListWidgetItem *nodeItem = new QListWidgetItem( this );
    QPixmap pixmap(32,32);
    pixmap = QPixmap(g_NodeSVGResourcePaths[i]).scaled( 32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    nodeItem->setIcon( QIcon(pixmap) );
    nodeItem->setData( Qt::UserRole + 0, pixmap );
    nodeItem->setData( Qt::UserRole + 1, i );
    nodeItem->setData( Qt::UserRole + 2, -1 );
    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( g_NodeNames[i] );
  }

}

/*
void BehaviorTreeList::addPiece( QPixmap pixmap, QPoint location )
{
  QListWidgetItem *pieceItem = new QListWidgetItem( this );
  pieceItem->setIcon( QIcon( pixmap ) );
  pieceItem->setData( Qt::UserRole, QVariant( pixmap ) );
  pieceItem->setData( Qt::UserRole + 1, location );
  pieceItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
      | Qt::ItemIsDragEnabled );
  pieceItem->setText( "Banan" );
}
*/

void BehaviorTreeList::startDrag( Qt::DropActions /*supportedActions*/ )
{
  QListWidgetItem *item = currentItem();

  QByteArray itemData;
  QDataStream dataStream( &itemData, QIODevice::WriteOnly );

  QPixmap pixmap = qVariantValue<QPixmap> ( item->data( Qt::UserRole + 0 ) );

  dataStream << item->data( Qt::UserRole + 1 ).toInt();
  dataStream << item->data( Qt::UserRole + 2 ).toInt();

  QMimeData *mimeData = new QMimeData;
  mimeData->setData( "ctstudio/x-node", itemData );

  QDrag *drag = new QDrag( this );
  drag->setMimeData( mimeData );
  drag->setHotSpot( QPoint( pixmap.width(), pixmap.height() ) );
  drag->setPixmap( pixmap );

  drag->exec( Qt::CopyAction );
}
