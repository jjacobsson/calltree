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
#include "../x-node.h"

#include <btree/btree_func.h>

BehaviorTreeList::BehaviorTreeList( QWidget *parent ) :
  QListWidget( parent )
{
  setDragEnabled( true );
  setViewMode( QListView::ListMode );
  setIconSize( QSize( 16, 16 ) );
  setSpacing( 2 );
  setAcceptDrops( false );
  setDropIndicatorShown( false );

  setupStandardNodes();
}

void BehaviorTreeList::loadSymbols( BehaviorTreeContext ctx )
{
  clear();
  setupStandardNodes();

  int c;
  NamedSymbol* ns = BehaviorTreeContextAccessSymbols( ctx, &c );

  XNodeData node_data;

  for( int i = 0; i < c; ++i )
  {
    bool set = false;
    const char* name = 0x0;
    switch( ns[i].m_Type )
    {
    case E_ST_ACTION:
      if( ns[i].m_Symbol.m_Action->m_Declared )
      {
        node_data.m_Type = E_XNDT_NODE;
        node_data.m_NodeGrist = E_GRIST_ACTION;
        node_data.m_FuncId = ns[i].m_Symbol.m_Action->m_Id.m_Hash;
        name = ns[i].m_Symbol.m_Action->m_Id.m_Text;
        set = true;
      }
      break;
    case E_ST_DECORATOR:
      if( ns[i].m_Symbol.m_Decorator->m_Declared )
      {
        node_data.m_Type = E_XNDT_NODE;
        node_data.m_NodeGrist = E_GRIST_DECORATOR;
        node_data.m_FuncId = ns[i].m_Symbol.m_Decorator->m_Id.m_Hash;
        name = ns[i].m_Symbol.m_Decorator->m_Id.m_Text;
        set = true;
      }
      break;
    }
    if( !set )
      continue;

    QListWidgetItem *nodeItem = new QListWidgetItem( this );
    QPixmap pixmap(32,32);

    pixmap = QPixmap(g_NodeSVGResourcePaths[node_data.m_NodeGrist]).scaled(
      32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
    );

    nodeItem->setIcon( QIcon(pixmap) );
    nodeItem->setData( Qt::UserRole + 0, pixmap );
    nodeItem->setData( Qt::UserRole + 1,
      QByteArray( (const char*)(&node_data), sizeof(XNodeData)) );

    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( name );
  }
}

void BehaviorTreeList::startDrag( Qt::DropActions /*supportedActions*/ )
{
  QListWidgetItem *item = currentItem();


  QPixmap pixmap = qVariantValue<QPixmap> ( item->data( Qt::UserRole + 0 ) );
  QMimeData *mimeData = new QMimeData;
  mimeData->setData( "ctstudio/x-node", item->data( Qt::UserRole + 1 ).toByteArray() );

  QDrag *drag = new QDrag( this );
  drag->setMimeData( mimeData );
  drag->setHotSpot( QPoint( pixmap.width(), pixmap.height() ) );
  //drag->setPixmap( pixmap );

  drag->exec( Qt::CopyAction );
}

void BehaviorTreeList::setupStandardNodes()
{
  XNodeData node_data;

  {
    QListWidgetItem *nodeItem = new QListWidgetItem( this );
    QPixmap pixmap(32,32);
    pixmap = QPixmap( ":/nodes/tree.svg" ).scaled(
      32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
    );
    nodeItem->setIcon( QIcon(pixmap) );
    nodeItem->setData( Qt::UserRole + 0, pixmap );

    node_data.m_Type      = E_XNDT_TREE;
    node_data.m_NodeGrist = E_GRIST_UNKOWN;
    node_data.m_FuncId    = 0;

    nodeItem->setData( Qt::UserRole + 1,
      QByteArray( (const char*)(&node_data), sizeof(XNodeData)) );

    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( tr("Tree") );
  }

  for( int i = 1; i < E_GRIST_DECORATOR; ++i )
  {
    QListWidgetItem *nodeItem = new QListWidgetItem( this );
    QPixmap pixmap(32,32);

    pixmap = QPixmap(g_NodeSVGResourcePaths[i]).scaled(
      32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
    );

    nodeItem->setIcon( QIcon(pixmap) );
    nodeItem->setData( Qt::UserRole + 0, pixmap );

    node_data.m_Type        = E_XNDT_NODE;
    node_data.m_NodeGrist   = i;
    node_data.m_FuncId      = 0;

    nodeItem->setData( Qt::UserRole + 1,
      QByteArray( (const char*)(&node_data), sizeof(XNodeData)) );

    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( g_NodeNames[i] );
  }
}
