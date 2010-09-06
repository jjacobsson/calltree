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

#ifndef PARAMETERNAMEEDIT_H_INCLUDED
#define PARAMETERNAMEEDIT_H_INCLUDED

#include <btree/btree_data.h>

#include <QtGui/QLineEdit>

class ParameterNameEdit: public QLineEdit
{
Q_OBJECT
public:

  ParameterNameEdit( BehaviorTreeContext, Parameter* param );

  Parameter* getParameter() { return m_Param; }

signals:

  void changed();

protected:

  bool handleTextChange();

  void keyPressEvent( QKeyEvent* event );

private:

  BehaviorTreeContext m_Context;
  Parameter* m_Param;

};

#endif /* PARAMETERNAMEEDIT_H_INCLUDED */
