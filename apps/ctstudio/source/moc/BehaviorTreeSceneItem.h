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

#ifndef BEHAVOIRTREESCENEITEM_H_INCLUDED
#define BEHAVOIRTREESCENEITEM_H_INCLUDED

#include <QtSvg/QGraphicsSvgItem>
#include <QtCore/QList>

#include "../GraphicsItemTypes.h"

// Internal Forward declares
class NodeToNodeArrow;

class BehaviorTreeSceneItem: public QGraphicsSvgItem
{
Q_OBJECT
public:

  enum
  {
    Type = BehaviorTreeSceneItemType
  };

  int type() const
  {
    return Type;
  }

  BehaviorTreeSceneItem( const char* gfx_path, BehaviorTreeSceneItem* parent = 0x0 );

  virtual bool isType( int type ) const
  {
    if( Type == type )
      return true;
    return false;
  }

  void removeArrow( NodeToNodeArrow* arrow );
  void removeArrows();
  void addArrow( NodeToNodeArrow* arrow );

  NodeToNodeArrow* findArrowTo( BehaviorTreeSceneItem* other );

signals:

  void itemDragged();

protected:

  QVariant itemChange( GraphicsItemChange change, const QVariant &value );

  void mousePressEvent( QGraphicsSceneMouseEvent* event );
  void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
  void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

  virtual void dragMove();
  virtual void dragBegin();
  virtual void dragEnd();

private:

  enum MouseState
  {
    E_MS_NONE, E_MS_LB_DOWN, E_MS_DRAGGING
  };

  MouseState m_MouseState; // Current state of mouse handling.
  QPoint m_StartPos; // The event's screen position when the LMB was pressed;

protected:
  QList<NodeToNodeArrow*> m_Arrows;
};

#endif
