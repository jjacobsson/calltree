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

#include "ParameterNameEdit.h"

#include <QtGui/QPalette>
#include <QtGui/QKeyEvent>

#include "../PreventDuplicateSymbolsValidator.h"

#include <other/lookup3.h>

ParameterNameEdit::ParameterNameEdit( BehaviorTreeContext ctx, Parameter* p ) :
  QLineEdit( p->m_Id.m_Text ), m_Context( ctx ), m_Param( p )
{
  setValidator( new PreventDuplicateSymbols( m_Context, this ) );
  setFrame( false );

  QPalette pal = palette();
  pal.setColor( backgroundRole(), Qt::transparent );
  setPalette( pal );
}

bool ParameterNameEdit::handleTextChange()
{
  QString t = text();

  //Empty text... not valid.
  if( t.isEmpty() )
    return false;

  unsigned int hash = hashlittle( t.toAscii().constData() );

  //Text has not changed, just early out.
  if( hash == m_Param->m_Id.m_Hash )
    return true;

  //Text has changed, handle that and signal the change.
  m_Param->m_Id.m_Hash = hash;
  m_Param->m_Id.m_Text = register_string( m_Context, t.toAscii().constData(), hash );

  //emit
  emit changed();

  return true;
}

void ParameterNameEdit::keyPressEvent( QKeyEvent* e )
{
  QLineEdit::keyPressEvent( e );
  switch( e->key() )
  {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    if( handleTextChange() )
      clearFocus();
    break;
  }
}
