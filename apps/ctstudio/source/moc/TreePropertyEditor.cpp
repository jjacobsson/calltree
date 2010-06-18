/*******************************************************************************
 * Copyright (c) 2010-05-31 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "TreePropertyEditor.h"

#include <QtGui/QFormLayout>
#include <QtGui/QStandardItem>
#include <QtGui/QHeaderView>
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include "ParameterNameEdit.h"
#include "ParameterTypeEdit.h"

#include <other/lookup3.h>

#include "../IconCache.h"

TreePropertyEditor::TreePropertyEditor( BehaviorTreeContext ctx, BehaviorTree* t, QWidget* parent )
  : QTableWidget( parent )
  , m_Context( ctx )
  , m_Tree( t )
{
  setWordWrap( false );
  setColumnCount( 2 );

  verticalHeader()->setVisible( false );

  QStringList headers;
  headers.push_back( tr( "Name" ) );
  headers.push_back( tr( "Type" ) );
  setHorizontalHeaderLabels( headers );

  setupItemsFromDeclaration( t->m_Declarations );

  setSelectionMode( QAbstractItemView::NoSelection );
  setAlternatingRowColors( true );
}

TreePropertyEditor::~TreePropertyEditor()
{
}

void TreePropertyEditor::childChanged()
{
  emit changed();
}

void TreePropertyEditor::deleteProperty()
{
  int index = m_DeleteButtons.indexOf( sender() );
  if( index > 0 )
  {
    ParameterTypeEdit* pte = static_cast<ParameterTypeEdit*>( cellWidget(index,1) );
    hash_t id = pte->getParameter()->m_Id.m_Hash;
    remove_declaration( m_Context, m_Tree, id );
    setupItemsFromDeclaration( m_Tree->m_Declarations );
    emit changed();
  }
}

void TreePropertyEditor::createProperty()
{
  char param_name_buff[128];
  unsigned int c = 0;
  sprintf( param_name_buff, "a_%08u", c );
  hash_t name_hash = hashlittle( param_name_buff );
  Parameter* p = find_by_hash( m_Tree->m_Declarations, name_hash );
  while( p )
  {
    sprintf( param_name_buff, "a_%08u", ++c );
    name_hash = hashlittle( param_name_buff );
    p = find_by_hash( m_Tree->m_Declarations, name_hash );
  }

  p = (Parameter*)allocate_object( m_Context );
  init( p );
  p->m_Id.m_Text = register_string( m_Context, param_name_buff );
  p->m_Id.m_Hash = name_hash;
  p->m_Type = E_VART_INTEGER;

  if( !m_Tree->m_Declarations )
    m_Tree->m_Declarations = p;
  else
    append_to_end( m_Tree->m_Declarations, p );

  setupItemsFromDeclaration( m_Tree->m_Declarations );
  emit changed();
}

void TreePropertyEditor::setupItemsFromDeclaration( Parameter* first )
{
  m_DeleteButtons.clear();
  clearContents();
  clearSpans();

  QIcon* icon = IconCache::get(":/nodes/fail.svg");
  QSize icon_size;
  icon_size.setHeight( 12 );
  icon_size.setWidth( 12 );
  if( !icon )
    return;

  Parameter* it = first;
  setRowCount( count_elements( it ) + 1 );
  int row = 0;
  while( it )
  {
    QWidget* cellwidget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QToolButton* dbtn = new QToolButton();
    ParameterNameEdit* name = new ParameterNameEdit( m_Context, it );
    ParameterTypeEdit* type = new ParameterTypeEdit( m_Context, it );

    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( dbtn );
    layout->addWidget( name );
    cellwidget->setLayout( layout );

    dbtn->setIcon( *icon );
    dbtn->setIconSize( icon_size );
    dbtn->setToolButtonStyle( Qt::ToolButtonIconOnly );

    connect( dbtn, SIGNAL( clicked() ), this, SLOT( deleteProperty() ) );
    connect( name, SIGNAL( changed() ), this, SLOT( childChanged() ) );
    connect( type, SIGNAL( changed() ), this, SLOT( childChanged() ) );

    m_DeleteButtons.append( dbtn );

    setCellWidget( row, 0, cellwidget );
    setCellWidget( row, 1, type );
    ++row;
    it = it->m_Next;
  }

  QWidget* cellwidget = new QWidget;
  QHBoxLayout* layout = new QHBoxLayout;
  QPushButton* button = new QPushButton( tr("Add Parameter") );

  cellwidget->setLayout( layout );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setAlignment( Qt::AlignLeft );
  layout->addWidget( button );
  button->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );

  connect( button, SIGNAL(clicked()), this, SLOT(createProperty()) );

  setSpan( row, 0, 1, 2 );
  setCellWidget( row, 0, cellwidget );

  resizeRowsToContents();

  QHeaderView* hw = horizontalHeader();
  hw->setMinimumSectionSize( 65 );
  hw->setResizeMode( QHeaderView::ResizeToContents );
  hw->setStretchLastSection( true );
  hw->setHighlightSections( false );
  hw->setClickable( false );
}
