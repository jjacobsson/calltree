/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BEHAVOIRTREELIST_H_INCLUDED
#define BEHAVOIRTREELIST_H_INCLUDED

#include <QtGui/QListWidget>

class BehaviorTreeList : public QListWidget
{
   Q_OBJECT

public:
   BehaviorTreeList(QWidget *parent = 0);

protected:
   void startDrag(Qt::DropActions supportedActions);
};

#endif
