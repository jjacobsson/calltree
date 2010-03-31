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

#include "NodePropertyEditor.h"

#include <QtGui/QFormLayout>
#include <QtGui/QStandardItem>
#include <QtGui/QHeaderView>
#include <QtGui/QStyledItemDelegate>

#include "../IconCache.h"

#include "NodePropertyDelegate.h"

NodePropertyEditor::NodePropertyEditor( BehaviorTreeContext ctx, Node* n,
  QWidget* parent ) :
  QTableView( parent ), m_HasBuggs( false ), m_Context( ctx ), m_Node( n )
{
  setModel( &m_Model );
  setWordWrap( false );
  m_Delegate = new NodePropertyDelegate( this );

  m_Model.setColumnCount( 3 );
  setItemDelegateForColumn( 2, m_Delegate );
  verticalHeader()->setVisible( false );

  QStringList headers;
  headers.push_back( tr( "Param" ) );
  headers.push_back( tr( "Type" ) );
  headers.push_back( tr( "Value" ) );
  m_Model.setHorizontalHeaderLabels( headers );

  Parameter* params = get_parameters( m_Node );
  Parameter* declar = get_declarations( m_Node );

  if( params && declar )
    setupPropertyEditorForParamaters( params, declar );

  resizeRowsToContents();
  setSelectionMode( QAbstractItemView::NoSelection );

  QHeaderView* hw = horizontalHeader();
  hw->setMinimumSectionSize( 65 );
  hw->setResizeMode( QHeaderView::ResizeToContents );
  hw->setStretchLastSection( true );
  hw->setHighlightSections( false );
  hw->setClickable( false );

  connect(
  &m_Model, SIGNAL( itemChanged( QStandardItem* ) ), this,
    SLOT( updateNodeParameterData( QStandardItem* ) )
  );

}

NodePropertyEditor::~NodePropertyEditor()
{
  setModel( 0x0 );
  delete m_Delegate;
  m_Delegate = 0x0;
}

bool NodePropertyEditor::hasBuggs() const
{
  return m_HasBuggs;
}

void NodePropertyEditor::updateNodeParameterData( QStandardItem* item )
{
  int hash = item->data( Qt::UserRole + 0 ).toInt();
  int type = item->data( Qt::UserRole + 1 ).toInt();
  bool bugged = item->data( Qt::UserRole + 3 ).toBool();
  int row = item->data( Qt::UserRole + 4 ).toInt();

  Parameter* p = find_by_hash( get_parameters( m_Node ), hash );
  Parameter* d = find_by_hash( get_declarations( m_Node ), hash );

  if( !d )
    return;

  if( !p )
  {
    p = clone( m_Context, d );
    append_to_end( get_parameters( m_Node ), p );
    p->m_ValueSet = true;
    if( d->m_Type == E_VART_HASH )
      p->m_Type = E_VART_STRING;
  }

  if( bugged )
  {
    m_Model.item( row, 0 )->setIcon( QIcon() );
    item->setData( false, Qt::UserRole + 3 );
  }

  bool changed = false;

  switch( type )
  {
  case E_VART_BOOL:
    {
      bool t = p->m_Data.m_Bool;
      p->m_Data.m_Bool = item->checkState() == Qt::Checked ? true : false;
      if( t != p->m_Data.m_Bool )
        changed = true;
    }
    break;
  case E_VART_INTEGER:
    {
      int t = p->m_Data.m_Integer;
      p->m_Data.m_Integer = item->data( Qt::UserRole + 2 ).toInt();
      if( t != p->m_Data.m_Integer )
        changed = true;
    }
    break;
  case E_VART_FLOAT:
    {
      float t = p->m_Data.m_Float;
      p->m_Data.m_Float = item->data( Qt::UserRole + 2 ).toFloat();
      if( t != p->m_Data.m_Float )
        changed = true;
    }
    break;
  case E_VART_HASH:
  case E_VART_STRING:
    {
      const char* t = p->m_Data.m_String.m_Raw;
      p->m_Data.m_String.m_Parsed = register_string( m_Context,
        item->text().toAscii().constData() );
      p->m_Data.m_String.m_Raw = p->m_Data.m_String.m_Parsed;
      if( t != p->m_Data.m_String.m_Raw )
        changed = true;
    }
    break;
  }

  if( changed )
  {
    m_HasBuggs = false;
    int c = m_Model.rowCount();
    for( int i = 0; i < c; ++i )
    {
      if( !m_Model.item( i, 2 )->data( Qt::UserRole + 3 ).toBool() )
        continue;
      m_HasBuggs = true;
      break;
    }

    emit nodeParametersChanged();
  }
}

void NodePropertyEditor::setupPropertyEditorForParamaters( Parameter* set,
  Parameter* dec )
{
  QIcon* bug_icon = IconCache::get( ":/icons/bug.svg" );
  m_HasBuggs = false;
  Parameter* it = dec;
  while( it )
  {
    Parameter* v = find_by_hash( set, it->m_Id.m_Hash );
    bool param_bugged = false;

    if( !v )
      param_bugged = true;

    QStandardItem* l = new QStandardItem( it->m_Id.m_Text );
    QStandardItem* tl = new QStandardItem;
    QStandardItem* e = new QStandardItem;

    l->setFlags( Qt::NoItemFlags );
    tl->setFlags( Qt::NoItemFlags );
    e->setFlags( Qt::NoItemFlags );

    e->setData( it->m_Id.m_Hash, Qt::UserRole + 0 );
    e->setData( it->m_Type, Qt::UserRole + 1 );
    e->setData( false, Qt::UserRole + 3 );

    switch( it->m_Type )
    {
    case E_VART_BOOL:
      {
        tl->setText( tr( "bool" ) );
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
        if( v && as_bool( *v ) )
          e->setCheckState( Qt::Checked );
        else
          e->setCheckState( Qt::Unchecked );
      }
      break;
    case E_VART_FLOAT:
      {
        tl->setText( tr( "float" ) );
        if( v )
        {
          e->setText( QString( "%1" ).arg( as_float( *v ) ) );
          e->setData( as_float( *v ), Qt::UserRole + 2 );
        }
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      }
      break;
    case E_VART_INTEGER:
      {
        tl->setText( tr( "integer" ) );
        if( v )
        {
          e->setText( QString( "%1" ).arg( as_integer( *v ) ) );
          e->setData( as_integer( *v ), Qt::UserRole + 2 );
        }
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      }
      break;
    case E_VART_HASH:
      tl->setText( tr( "hash" ) );
      if( v )
      {
        if( v->m_Type == E_VART_STRING )
          e->setText( as_string( *v )->m_Raw );
        else if( v->m_Type == E_VART_INTEGER || v->m_Type == E_VART_HASH )
        {
          v->m_Type = E_VART_STRING;
          v->m_Data.m_String.m_Parsed = register_string( m_Context, "" );
          v->m_Data.m_String.m_Raw = register_string( m_Context, "" );
        }
      }
      e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      break;
    case E_VART_STRING:
      {
        tl->setText( tr( "string" ) );
        if( v )
          e->setText( as_string( *v )->m_Raw );
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      }
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      // Warning Killers
      param_bugged = true;
      tl->setText( tr( "I have no clue." ) );
      break;
    }

    int row = m_Model.rowCount();
    e->setData( row, Qt::UserRole + 4 );
    l->setFlags( Qt::ItemIsEnabled );
    tl->setFlags( Qt::ItemIsEnabled );

    m_Model.setRowCount( row + 1 );
    m_Model.setItem( row, 0, l );
    m_Model.setItem( row, 1, tl );
    m_Model.setItem( row, 2, e );

    if( bug_icon && param_bugged )
    {
      l->setIcon( *bug_icon );
      e->setData( true, Qt::UserRole + 3 );
    }
    if( param_bugged )
      m_HasBuggs = true;

    it = it->m_Next;
  }
}
