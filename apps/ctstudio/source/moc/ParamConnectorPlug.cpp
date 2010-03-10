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

#include "ParamConnectorPlug.h"

ParamConnectorPlug::ParamConnectorPlug( hash_t hash, QObject* parent )
  : QObject( parent )
  , m_Hash( hash )
{}

void ParamConnectorPlug::checkBoxChanged( int state )
{
  emit dataChanged( sender(), m_Hash );
}

void ParamConnectorPlug::lineEditChanged()
{
  emit dataChanged( sender(), m_Hash );
}


