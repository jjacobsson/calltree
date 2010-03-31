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

#ifndef BEHAVOIRTREETREE_H_INCLUDED
#define BEHAVOIRTREETREE_H_INCLUDED

#include "BehaviorTreeSceneItem.h"
#include "../GraphicsItemTypes.h"

class QGraphicsSvgItem;
class QGraphicsTextItem;

struct BehaviorTree;

class BehaviorTreeTree: public BehaviorTreeSceneItem
{
  Q_OBJECT

public:

  BehaviorTreeTree( BehaviorTreeContext ctx, BehaviorTree* tree );
  ~BehaviorTreeTree();

  enum
  {
    Type = BehaviorTreeTreeType
  };

  bool isType( int type ) const {
    if( Type == type )
      return true;
    return BehaviorTreeSceneItem::isType( type );
  }

  BehaviorTree* GetTree() { return m_Tree; }

  QRectF boundingRect() const;
  QPointF iconPosition() const;
  QRectF layoutBoundingRect() const;
  qreal layoutOffset() const;

  BehaviorTreeSceneItem* firstChild();

  void dragEnd();

signals:

  void symbolsChanged();

private slots:

  void updateName();

protected:

  void setupLabel( const char* str );
  void setupPropertyEditor();

  BehaviorTree* m_Tree;
  QGraphicsSvgItem* m_Graphics;
  QGraphicsTextItem* m_Label;
};

#endif
