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

#ifndef TREEPROPERTYEDITOR_H_INCLUDED
#define TREEPROPERTYEDITOR_H_INCLUDED

#include <QtCore/QList>
#include <QtGui/QTableWidget>
#include <btree/btree.h>

class TreePropertyDelegate;

class QObject;

class TreePropertyEditor : public QTableWidget
{

Q_OBJECT

public:

  enum UserDataIndex
  {
    E_HASH = Qt::UserRole,
    E_TYPE
  };

  TreePropertyEditor( BehaviorTreeContext, BehaviorTree*, QWidget* parent = 0x0 );
  ~TreePropertyEditor();

signals:
  void changed();

private slots:

  void childChanged();
  void deleteProperty();
  void createProperty();

private:

  void setupItemsFromDeclaration( Parameter* );

  BehaviorTreeContext m_Context;
  BehaviorTree* m_Tree;
  QList<QObject*> m_DeleteButtons;
};

#endif /* TREEPROPERTYEDITOR_H_INCLUDED */
