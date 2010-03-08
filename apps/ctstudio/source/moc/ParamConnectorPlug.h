/*******************************************************************************
 * Copyright (c) 2010-03-08 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef PARAMCONNECTORPLUG_H_INCLUDED
#define PARAMCONNECTORPLUG_H_INCLUDED

#include <btree/btree_data.h>

#include <QtCore/QObject>

class QString;

class ParamConnectorPlug : public QObject
{
  Q_OBJECT
public:

  ParamConnectorPlug( hash_t hash, QObject* parent );

signals:

  void dataChanged( QObject* edit_widget, hash_t param_hash );

public slots:

  void checkBoxChanged( int state );
  void lineEditChanged( const QString& text );

private:
  const hash_t m_Hash;
};

#endif /* PARAMCONNECTORPLUG_H_INCLUDED */
