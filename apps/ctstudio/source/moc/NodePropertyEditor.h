/*******************************************************************************
 * Copyright (c) 2010-03-31 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef NODEPROPERTYEDITOR_H_INCLUDED
#define NODEPROPERTYEDITOR_H_INCLUDED

#include <QtGui/QTableView>
#include <QtGui/QStandardItemModel>

#include <btree/btree.h>

class NodePropertyDelegate;
class QStyledItemDelegate;
class QStandardItem;

class NodePropertyEditor : public QTableView
{

Q_OBJECT

public:

  enum UserDataIndex
  {
    E_HASH = Qt::UserRole,
    E_TYPE,
    E_DATA,
    E_ROW
  };

  NodePropertyEditor( BehaviorTreeContext, Node*, QWidget* parent = 0x0 );
  ~NodePropertyEditor();

  bool hasBuggs() const;

signals:

  void nodeParametersChanged();

private slots:

  void updateNodeParameterData( QStandardItem* );

protected:

  void setupPropertyEditorForParamaters( Parameter*, Parameter* );
  void validateParameters();

private:

  bool m_HasBuggs;
  bool m_HasWarnings;
  BehaviorTreeContext m_Context;
  Node* m_Node;

  NodePropertyDelegate* m_Delegate;
  QStandardItemModel m_Model;
};

#endif /* NODEPROPERTYEDITOR_H_INCLUDED */
