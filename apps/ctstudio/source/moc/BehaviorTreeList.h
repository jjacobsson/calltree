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

#ifndef BEHAVOIRTREELIST_H_INCLUDED
#define BEHAVOIRTREELIST_H_INCLUDED

#include <QtGui/QListWidget>

#include <btree/btree_data.h>

class BehaviorTreeList : public QListWidget
{
   Q_OBJECT

public:
   BehaviorTreeList(QWidget *parent = 0);

public slots:
   void loadSymbols( BehaviorTreeContext );

protected:
   void startDrag(Qt::DropActions supportedActions);
};

#endif
