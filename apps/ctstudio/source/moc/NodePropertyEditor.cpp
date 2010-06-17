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
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTableWidgetItem>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QComboBox>

#include "../IconCache.h"
#include "../standard_resources.h"

#include <float.h>

#include <other/lookup3.h>

typedef QWidget* (*EditorFactoryFunction)( Parameter* o, Parameter* p, Parameter* d );

QVariant ParameterValueAsVariant( Parameter* p )
{
  QVariant r;
  if( p )
  {
    switch( p->m_Type )
    {
    case E_VART_INTEGER:
      r = QVariant( as_integer( *p ) );
      break;
    case E_VART_FLOAT:
      r = QVariant( as_float( *p ) );
      break;
    case E_VART_STRING:
      r = QVariant( as_string( *p )->m_Parsed );
      break;
    case E_VART_BOOL:
      r = QVariant( as_bool( *p ) );
      break;
    case E_VART_HASH:
      r = QVariant( as_integer( *p ) );
      break;
    }
  }
  return r;
}

QWidget* CreateEnumEditor( Parameter* o, Parameter* p, Parameter* d )
{
  QComboBox* cb = new QComboBox;
  Parameter* e = find_child_by_hash( o, hashlittle( "e" ) );
  unsigned int label_hash = hashlittle( "label" );
  unsigned int value_hash = hashlittle( "value" );

  QVariant current_value_variant( ParameterValueAsVariant(p) );
  if( !current_value_variant.isValid() )
    current_value_variant = QVariant( 0 );

  int index = 0;

  while( e )
  {
    Parameter* label = find_child_by_hash( e, label_hash );
    Parameter* value = find_child_by_hash( e, value_hash );

    if( !label || !safe_to_convert( label, E_VART_STRING ) )
      continue;

    QVariant value_variant( ParameterValueAsVariant(value) );
    if( !value_variant.isValid() )
      value_variant = QVariant( cb->count() );

    cb->addItem( as_string( *label )->m_Parsed, value_variant );

    if( value_variant == current_value_variant )
      index = cb->count() - 1;

    e = e->m_Next;
  }

  cb->setCurrentIndex( index );

  return cb;
}

QWidget* CreateIntSpinboxEditor( Parameter* o, Parameter* p, Parameter* d )
{
  QSpinBox* sb = new QSpinBox;
  sb->setRange( INT_MIN, INT_MAX );
  if( p && safe_to_convert( p, E_VART_INTEGER ) )
    sb->setValue( as_integer( *p ) );

  Parameter* min = find_child_by_hash( o, hashlittle( "min" ) );
  Parameter* max = find_child_by_hash( o, hashlittle( "max" ) );
  Parameter* step = find_child_by_hash( o, hashlittle( "step" ) );

  if( min && safe_to_convert( min, E_VART_INTEGER ) )
    sb->setMinimum( as_integer( *min ) );
  if( max && safe_to_convert( max, E_VART_INTEGER ) )
    sb->setMaximum( as_integer( *max ) );
  if( step && safe_to_convert( step, E_VART_INTEGER ) )
    sb->setSingleStep( as_integer( *step ) );
  return sb;
}

QWidget* CreateFloatSpinboxEditor( Parameter* o, Parameter* p, Parameter* d )
{
  QDoubleSpinBox* sb = new QDoubleSpinBox;
  sb->setRange( -FLT_MAX, FLT_MAX );
  sb->setDecimals( 4 );
  if( p && safe_to_convert( p, E_VART_FLOAT ) )
    sb->setValue( as_float( *p ) );

  Parameter* min = find_child_by_hash( o, hashlittle( "min" ) );
  Parameter* max = find_child_by_hash( o, hashlittle( "max" ) );
  Parameter* step = find_child_by_hash( o, hashlittle( "step" ) );

  if( min && safe_to_convert( min, E_VART_FLOAT ) )
    sb->setMinimum( as_float( *min ) );
  if( max && safe_to_convert( max, E_VART_FLOAT ) )
    sb->setMaximum( as_float( *max ) );
  if( step && safe_to_convert( step, E_VART_FLOAT ) )
    sb->setSingleStep( as_float( *step ) );

  return sb;
}

NodePropertyEditor::NodePropertyEditor( BehaviorTreeContext ctx, Node* n, QWidget* parent )
  : QTableWidget( parent )
  , m_HasBuggs( false )
  , m_Context( ctx )
  , m_Node( n )
{
  setWordWrap( false );
  setColumnCount( 3 );

  verticalHeader()->setVisible( false );

  QStringList headers;
  headers.push_back( tr( "Name" ) );
  headers.push_back( tr( "Type" ) );
  headers.push_back( tr( "Value" ) );
  setHorizontalHeaderLabels( headers );

  Parameter* dec = get_declarations( m_Node );
  Parameter* par = get_parameters( m_Node );
  Parameter* opt = get_options( m_Node );
  opt = find_by_hash( opt, hashlittle( "cts" ) );
  opt = find_child_by_hash( opt, hashlittle( "params" ) );

  setupProperties( opt, par, dec );

  setSelectionMode( QAbstractItemView::NoSelection );
  setAlternatingRowColors( true );
}

NodePropertyEditor::~NodePropertyEditor()
{
}

bool NodePropertyEditor::hasBuggs() const
{
  return m_HasBuggs;
}

void NodePropertyEditor::setupProperties(
    Parameter* options,
    Parameter* params,
    Parameter* declarations
  )
{
  clearContents();

  QIcon* good_icon = IconCache::get(":/nodes/succeed.svg");
  QIcon* bug_icon = IconCache::get(":/icons/bug.svg");

  Parameter* it = declarations;
  setRowCount( count_elements( it ) );
  int row = 0;
  while( it )
  {
    QTableWidgetItem* name_item = new QTableWidgetItem;
    QTableWidgetItem* type_item = new QTableWidgetItem;

    name_item->setText( it->m_Id.m_Text );
    name_item->setIcon( *bug_icon );
    name_item->setFlags( Qt::ItemIsEnabled );

    type_item->setText( get_type_name( it ) );
    type_item->setFlags( Qt::ItemIsEnabled );

    setItem( row, 0, name_item );
    setItem( row, 1, type_item );

    Parameter* p_opts = find_child_by_hash( options, it->m_Id.m_Hash );
    Parameter* p = find_by_hash( params, it->m_Id.m_Hash );

    QWidget* edit = createEditor( p_opts, p, it );

    setCellWidget( row, 2, edit );

    ++row;
    it = it->m_Next;
  }

  resizeRowsToContents();

  QHeaderView* hw = horizontalHeader();
  hw->setMinimumSectionSize( 85 );
  hw->setResizeMode( QHeaderView::ResizeToContents );
  hw->setStretchLastSection( true );
  hw->setHighlightSections( false );
  hw->setClickable( false );
}

QWidget* NodePropertyEditor::createEditor(
  Parameter* options,
  Parameter* parameter,
  Parameter* declaration
)
{
  QWidget* r = 0x0;

  if( options && options->m_Type == E_VART_LIST && options->m_Data.m_List != 0x0 )
  {
    const unsigned int edit_types[] = {
      hashlittle( "enum" ),
      hashlittle( "int32_spinbox"),
      hashlittle( "float_spinbox" )
    };

    EditorFactoryFunction edit_funcs[] = {
      &CreateEnumEditor,
      &CreateIntSpinboxEditor,
      &CreateFloatSpinboxEditor
    };

    const int edit_type_count = sizeof( edit_types ) / sizeof( unsigned int );

    Parameter* edit_type = options->m_Data.m_List;
    while( edit_type && !r )
    {
      for( int i = 0; i < edit_type_count; ++i )
      {
        if( edit_type->m_Id.m_Hash != edit_types[i] )
          continue;

        r = edit_funcs[i]( edit_type, parameter, declaration );
        break;
      }
      edit_type = edit_type->m_Next;
    }
  }
  else
  {

  }

  return r;
}

void NodePropertyEditor::validateParameters()
{
}
