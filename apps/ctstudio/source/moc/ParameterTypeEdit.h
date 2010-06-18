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

#ifndef PARAMETERTYPEEDIT_H_INCLUDED
#define PARAMETERTYPEEDIT_H_INCLUDED

#include <btree/btree_data.h>

#include <QtGui/QComboBox>

class ParameterTypeEdit: public QComboBox
{
Q_OBJECT
public:

  ParameterTypeEdit( BehaviorTreeContext, Parameter* param );

  Parameter* getParameter() { return m_Param; }

signals:

  void changed();

protected slots:

  void indexChanged( int index );

private:

  BehaviorTreeContext m_Context;
  Parameter* m_Param;

};

#endif /* PARAMETERTYPEEDIT_H_INCLUDED */
