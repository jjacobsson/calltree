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

#include "BehaviorTreeList.h"
#include "../BehaviorTreeListItem.h"
#include "../standard_resources.h"
#include "../x-node.h"


#include <btree/btree_func.h>

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QPixmap>

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
  NamedSymbol* ns = access_symbols( ctx, &c );

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
    case E_ST_TREE:
      if( ns[i].m_Symbol.m_Tree->m_Declared )
      {
        node_data.m_Type = E_XNDT_NODE;
        node_data.m_NodeGrist = E_GRIST_TREE;
        node_data.m_FuncId = ns[i].m_Symbol.m_Tree->m_Id.m_Hash;
        name = ns[i].m_Symbol.m_Tree->m_Id.m_Text;
        set = true;
      }
      break;
    case E_ST_UNKOWN:
    case E_MAX_SYMBOL_TYPES:
      break;
    }
    if( !set )
      continue;

    QListWidgetItem *nodeItem = new BehaviorTreeListItem( node_data.m_NodeGrist );
    nodeItem->setIcon( QIcon(g_NodeIcons[node_data.m_NodeGrist]) );
    nodeItem->setData( Qt::UserRole + 0,
      QByteArray( (const char*)(&node_data), sizeof(XNodeData)) );

    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( name );
    addItem( nodeItem );
  }

  sortItems();
}

void BehaviorTreeList::startDrag( Qt::DropActions /*supportedActions*/ )
{
  QListWidgetItem *item = currentItem();

  QMimeData *mimeData = new QMimeData;
  mimeData->setData( "ctstudio/x-node", item->data( Qt::UserRole + 0 ).toByteArray() );

  QDrag *drag = new QDrag( this );
  drag->setMimeData( mimeData );
  drag->exec( Qt::CopyAction );
}

void BehaviorTreeList::setupStandardNodes()
{
  XNodeData node_data;

  {
    QListWidgetItem *nodeItem = new BehaviorTreeListItem( 0 );

    nodeItem->setIcon( QIcon(":/icons/tree.png") );
    node_data.m_Type      = E_XNDT_TREE;
    node_data.m_NodeGrist = E_GRIST_UNKOWN;
    node_data.m_FuncId    = 0;

    nodeItem->setData( Qt::UserRole + 0,
      QByteArray( (const char*)(&node_data), sizeof(XNodeData)) );

    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( tr("Tree") );
    addItem( nodeItem );
  }

  for( int i = 1; i < E_GRIST_TREE; ++i )
  {
    QListWidgetItem *nodeItem = new BehaviorTreeListItem( i );

    nodeItem->setIcon( QIcon(g_NodeIcons[i]) );

    node_data.m_Type        = E_XNDT_NODE;
    node_data.m_NodeGrist   = i;
    node_data.m_FuncId      = 0;

    nodeItem->setData( Qt::UserRole + 0,
      QByteArray( (const char*)(&node_data), sizeof(XNodeData)) );

    nodeItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable
        | Qt::ItemIsDragEnabled );
    nodeItem->setText( g_NodeNames[i] );
    addItem( nodeItem );
  }
}
