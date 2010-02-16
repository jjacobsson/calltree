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

#ifndef NODETONODEARROW_H_INCLUDED
#define NODETONODEARROW_H_INCLUDED

#include <QtGui/QGraphicsLineItem>

#include "GraphicsItemTypes.h"

// Qt Forward declares
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;

// BT Forward declares
class BehaviorTreeSceneItem;

class NodeToNodeArrow: public QGraphicsLineItem
{
public:
  enum
  {
    Type = NodeToNodeArrowType
  };

  NodeToNodeArrow( BehaviorTreeSceneItem *start, BehaviorTreeSceneItem *end,
    QGraphicsScene *scene = 0x0 );

  int type() const
  {
    return Type;
  }

  BehaviorTreeSceneItem *startItem() const
  {
    return m_Start;
  }

  BehaviorTreeSceneItem *endItem() const
  {
    return m_End;
  }

  void setStartAndEnd( BehaviorTreeSceneItem *start, BehaviorTreeSceneItem *end );
  void setDashed( bool dashed );

public slots:

  void updatePosition();

protected:

  void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
    QWidget *widget = 0 );

private:
  BehaviorTreeSceneItem *m_Start;
  BehaviorTreeSceneItem *m_End;
};

#endif
