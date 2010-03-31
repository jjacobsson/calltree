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
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleValidator>
#include <QtGui/QIntValidator>


#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>

#include "../IconCache.h"

NodePropertyEditor::NodePropertyEditor( BehaviorTreeContext ctx, Node* n,
  QWidget* parent ) :
  QTableWidget( parent ), m_HasBuggs( false ), m_Context( ctx ), m_Node( n )
{
  setColumnCount( 3 );

  QStringList headers;
  headers.push_back( tr( "Param" ) );
  headers.push_back( tr( "Type" ) );
  headers.push_back( tr( "Value" ) );
  setHorizontalHeaderLabels( headers );
  verticalHeader()->hide();

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
}

bool NodePropertyEditor::hasBuggs() const
{
  return m_HasBuggs;
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

    QTableWidgetItem* l  = new QTableWidgetItem( it->m_Id.m_Text );
    QTableWidgetItem* tl = new QTableWidgetItem;
    QTableWidgetItem* e  = new QTableWidgetItem;

    e->setData( Qt::UserRole, it->m_Id.m_Hash );

    switch( it->m_Type )
    {
    case E_VART_BOOL:
      {
        tl->setText( tr( "bool" ) );
        e->setFlags( Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsUserCheckable );
        e->setCheckState( Qt::Unchecked );
      }
      break;
    case E_VART_FLOAT:
      {
        tl->setText( tr("float") );
        if( v )
          e->setText( QString("%1").arg( as_float( *v ) ) );
      }
      break;
    case E_VART_INTEGER:
      {
        tl->setText( tr("integer") );
        if( v )
          e->setText( QString("%1").arg( as_integer( *v ) ) );
      }
      break;
    case E_VART_HASH:
      tl->setText( tr("hash") );
      if( v )
      {
        if( v->m_Type == E_VART_STRING )
          e->setText( as_string( *v )->m_Raw );
        else if( v->m_Type == E_VART_INTEGER || v->m_Type == E_VART_HASH )
          e->setText( QString("%1").arg( as_integer( *v ) ) );
      }
      break;
    case E_VART_STRING:
      {
        tl->setText( tr("string") );
        if( v )
          e->setText( as_string(*v)->m_Raw );
      }
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      // Warning Killers
      param_bugged = true;
      tl->setText( tr("I have no clue.") );
      break;
    }

    int row = rowCount();

    l->setFlags( Qt::ItemIsEnabled );
    tl->setFlags( Qt::ItemIsEnabled );

    setRowCount( row + 1 );
    setItem( row, 0, l );
    setItem( row, 1, tl );
    setItem( row, 2, e );

    if( bug_icon && param_bugged )
      item( row, 0 )->setIcon( *bug_icon );

    if( param_bugged )
      m_HasBuggs = true;

    it = it->m_Next;
  }
}
