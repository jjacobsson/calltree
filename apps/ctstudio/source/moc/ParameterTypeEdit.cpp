/*******************************************************************************
 * Copyright (c) 2010-06-04 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "ParameterTypeEdit.h"

ParameterTypeEdit::ParameterTypeEdit( BehaviorTreeContext ctx, Parameter* param ) :
  m_Context( ctx ), m_Param( param )
{
  addItem( tr("Integer") );
  addItem( tr("Float") );
  addItem( tr("String") );
  addItem( tr("Bool") );
  addItem( tr("Hash") );
  setCurrentIndex( m_Param->m_Type - 1 );
  connect( this, SIGNAL(currentIndexChanged( int )), this, SLOT(indexChanged( int )) );
}

void ParameterTypeEdit::indexChanged( int index )
{
  ParameterType ot = m_Param->m_Type;
  switch( index + 1 )
  {
  case E_VART_INTEGER:
    m_Param->m_Type = E_VART_INTEGER;
    break;
  case E_VART_FLOAT:
    m_Param->m_Type = E_VART_FLOAT;
    break;
  case E_VART_STRING:
    m_Param->m_Type = E_VART_STRING;
    break;
  case E_VART_BOOL:
    m_Param->m_Type = E_VART_BOOL;
    break;
  case E_VART_HASH:
    m_Param->m_Type = E_VART_HASH;
    break;
  default:
    m_Param->m_Type = E_VART_UNDEFINED;
    break;
  }
  if( ot != m_Param->m_Type )
  {
    emit changed();
  }
}

