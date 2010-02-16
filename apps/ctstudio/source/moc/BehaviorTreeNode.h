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

#ifndef BEHAVOIRTREENODE_H_INCLUDED
#define BEHAVOIRTREENODE_H_INCLUDED

#include "../GraphicsItemTypes.h"

#include "BehaviorTreeSceneItem.h"

// BT Forward declares
struct Node;

// Internal Forward declares
class NodeToNodeArrow;

class BehaviorTreeNode: public BehaviorTreeSceneItem
{
Q_OBJECT
public:

  enum
  {
    Type = BehaviorTreeNodeType
  };

  BehaviorTreeNode( Node* n, BehaviorTreeSceneItem* parent = 0x0 );

  int type() const
  {
    return Type;
  }

protected:

  void dragMove();
  void dragBegin();
  void dragEnd();

  void setupLabel();
  void setupTooltip();

private:

  void setupRelinkage();
  void executeRelinkage();
  void lookForRelinkTarget();

  Node* m_Node; // Associated behavior tree node.

  struct Relinkage
  {
    Node* m_Parent; // The new parent node of this item
    Node* m_Sibling; // The new sibling node of this item
    bool m_BeforeSibling; // If this is true this node should be linked in before the sibling, otherwise it should be linked after the sibling.
  };

  Relinkage m_Relinkage; // This is the information needed to be able to correctly link the m_Node into a new position in the BT.
  NodeToNodeArrow* m_DraggingArrow; // This arrow is the one used to indicate where the node will be linked while dragging.

};

#endif
