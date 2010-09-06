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
#include "../standard_resources.h"

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

  if( declar )
    setupPropertyEditorForParamaters( params, declar );

  validateParameters();

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

  int hash = item->data( E_HASH ).toInt();
  int type = item->data( E_TYPE ).toInt();
  int row = item->data( E_ROW ).toInt();
  bool new_param = false;

  Parameter* p = find_by_hash( get_parameters( m_Node ), hash );
  Parameter* d = find_by_hash( get_declarations( m_Node ), hash );

  if( !d )
    return;

  if( p )
  {
    if( !safe_to_convert( p, d->m_Type ) )
    {
      p->m_Type = d->m_Type;
      memset( &p->m_Data, 0, sizeof( ParameterData ) );
      if( d->m_Type == E_VART_HASH )
        p->m_Type = E_VART_STRING;
    }
  }
  else
  {
    new_param = true;
    p = clone( m_Context, d );

    if( get_parameters(m_Node) )
      append_to_end( get_parameters( m_Node ), p );
    else
      set_parameters( m_Node, p );

    p->m_ValueSet = true;
    if( d->m_Type == E_VART_HASH )
      p->m_Type = E_VART_STRING;
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
      p->m_Data.m_Integer = item->data( E_DATA ).toInt();
      if( t != p->m_Data.m_Integer )
        changed = true;
    }
    break;
  case E_VART_FLOAT:
    {
      float t = p->m_Data.m_Float;
      p->m_Data.m_Float = item->data( E_DATA ).toFloat();
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

  if( changed || new_param )
  {
    validateParameters();
    emit nodeParametersChanged();
  }
}

void NodePropertyEditor::setupPropertyEditorForParamaters( Parameter* set,
  Parameter* dec )
{
  Parameter* it = dec;
  while( it )
  {
    Parameter* v = find_by_hash( set, it->m_Id.m_Hash );

    QStandardItem* l = new QStandardItem( it->m_Id.m_Text );
    QStandardItem* tl = new QStandardItem;
    QStandardItem* e = new QStandardItem;

    l->setFlags( Qt::NoItemFlags );
    tl->setFlags( Qt::NoItemFlags );
    e->setFlags( Qt::NoItemFlags );

    e->setData( it->m_Id.m_Hash, E_HASH );
    e->setData( it->m_Type, E_TYPE );

    bool conv_safe = safe_to_convert( v, it->m_Type );

    switch( it->m_Type )
    {
    case E_VART_BOOL:
      {
        tl->setText( tr( "bool" ) );
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
        if( v && conv_safe && as_bool( *v ) )
          e->setCheckState( Qt::Checked );
        else
          e->setCheckState( Qt::Unchecked );
      }
      break;
    case E_VART_FLOAT:
      {
        tl->setText( tr( "float" ) );
        if( v && conv_safe )
        {
          e->setText( QString( "%1" ).arg( as_float( *v ) ) );
          e->setData( as_float( *v ), E_DATA );
        }
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      }
      break;
    case E_VART_INTEGER:
      {
        tl->setText( tr( "integer" ) );
        if( v && conv_safe )
        {
          e->setText( QString( "%1" ).arg( as_integer( *v ) ) );
          e->setData( as_integer( *v ), E_DATA );
        }
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      }
      break;
    case E_VART_HASH:
      tl->setText( tr( "hash" ) );
      if( v && conv_safe )
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
        if( v && conv_safe )
          e->setText( as_string( *v )->m_Raw );
        e->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
      }
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      // Warning Killers
      tl->setText( tr( "I have no clue." ) );
      break;
    }

    int row = m_Model.rowCount();
    e->setData( row, E_ROW );
    l->setFlags( Qt::ItemIsEnabled );
    tl->setFlags( Qt::ItemIsEnabled );

    m_Model.setRowCount( row + 1 );
    m_Model.setItem( row, 0, l );
    m_Model.setItem( row, 1, tl );
    m_Model.setItem( row, 2, e );

    it = it->m_Next;
  }
}

void NodePropertyEditor::validateParameters()
{
  QIcon* bug_icon  = IconCache::get( g_IconNames[ICON_BUG] );
  QIcon* good_icon = IconCache::get( g_IconNames[ICON_SUCCESS] );
  QIcon* warn_icon = IconCache::get( g_IconNames[ICON_WARNING] );

  Parameter* params = get_parameters( m_Node );
  Parameter* declar = get_declarations( m_Node );
  Parameter* it = declar;

  m_HasBuggs = false;
  m_HasWarnings = false;

  int row = 0;

  while( it )
  {
    QStandardItem* label = m_Model.item( row, 0 );
    Parameter* p = find_by_hash( params, it->m_Id.m_Hash );
    if( !p || !safe_to_convert( p, it->m_Type ) )
    {
      label->setIcon( *bug_icon );
      m_HasBuggs = true;
    }
    else
    {
      label->setIcon( *good_icon );
    }
    it = it->m_Next;
    ++row;
  }
}
