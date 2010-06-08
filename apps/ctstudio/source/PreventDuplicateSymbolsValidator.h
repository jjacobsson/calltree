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

#ifndef PREVENTDUPLICATESYMBOLSVALIDATOR_H_INCLUDED
#define PREVENTDUPLICATESYMBOLSVALIDATOR_H_INCLUDED

#include <QtGui/QRegExpValidator>

#include <btree/btree.h>
#include <other/lookup3.h>

class PreventDuplicateSymbols : public QRegExpValidator
{
public:
  PreventDuplicateSymbols( BehaviorTreeContext ctx, QObject* parent )
    : QRegExpValidator(  QRegExp( "([a-zA-Z_])([a-zA-Z0-9_])*"), parent )
    , m_Context( ctx )
  {}

  QValidator::State validate ( QString & input, int & pos ) const
  {
    QValidator::State s = QRegExpValidator::validate( input, pos );
    if( s == QValidator::Invalid )
      return s;
    hash_t hash = hashlittle( input.toAscii().constData() );
    NamedSymbol* ns = find_symbol( m_Context, hash );
    if( ns )
      return QValidator::Intermediate;

    if( is_btree_keyword( input.toAscii().constData() ) )
      return QValidator::Intermediate;

    return s;
  }
private:
  BehaviorTreeContext m_Context;
};


#endif /* PREVENTDUPLICATESYMBOLSVALIDATOR_H_INCLUDED */
