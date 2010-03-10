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

#ifndef BEHAVOIRTREESCENE_H_INCLUDED
#define BEHAVOIRTREESCENE_H_INCLUDED

#include <QtGui/QGraphicsScene>

#include <vector>

#include "../GraphicsItemTypes.h"

// Qt Forward declares
class QString;
class QGraphicsItem;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QMainWindow;
class QGraphicsItemGroup;
class QPointF;

// BT Forward declares
struct Node;
struct BehaviorTree;

class BehaviorTreeSceneItem;
typedef struct SBehaviorTreeContext* BehaviorTreeContext;

struct XNodeData;

class BehaviorTreeScene: public QGraphicsScene
{
Q_OBJECT
public:

  BehaviorTreeScene( QMainWindow* );
  ~BehaviorTreeScene();

  void newTree();
  bool readFile( const QString& fileName );
  bool writeFile( const QString& fileName );

  void dragEnterEvent( QDragEnterEvent *event );
  void dragLeaveEvent( QDragLeaveEvent *event );
  void dragMoveEvent( QDragMoveEvent *event, const QPointF& mapped_pos );
  void dropEvent( QDropEvent *event );

  BehaviorTreeContext getTreeContext()
  {
    return m_TreeContext;
  }

  BehaviorTreeContext getFullContext()
  {
    return m_FullContext;
  }

signals:
  void modified();
  void updatedSymbols( BehaviorTreeContext );
  void itemSelected( QWidget* );

public slots:
  void layout();
  void itemModified( bool geometry_changed );
  void deleteSelected();
  void nodeSelected( QWidget* );
  void updateClone();

protected:

  struct Extents
  {
    Extents() :
      l( 0.0f ), r( 0.0f )
    {
    }
    double l;
    double r;
  };

  typedef std::vector<Extents> ExtentsList;

  void createGraphics();
  void createGraphics( Node*, BehaviorTreeSceneItem* );

  void layoutRoot( BehaviorTreeSceneItem* n, ExtentsList& el );
  void depthFirstPlace( BehaviorTreeSceneItem* n, ExtentsList& pel );

  double minimumRootDistance( const ExtentsList& l, const ExtentsList& r );
  void moveExtents( ExtentsList& el, double dist );
  void mergeExtents( ExtentsList& res, const ExtentsList& lel,
    const ExtentsList& rel );
  void padExtents( ExtentsList& l, const ExtentsList& r );

  void setupDrag( const XNodeData& data );
  void setupTreeDrag( const XNodeData& data );
  void setupNodeDrag( const XNodeData& data );

  void setupDecoratorNode( Node*, const XNodeData& );
  void setupActionNode( Node*, const XNodeData& );
  void setupTreeNode( Node*, const XNodeData& );



  void destroySubTree( BehaviorTreeSceneItem* );

  BehaviorTreeContext m_TreeContext;
  BehaviorTreeContext m_FullContext;
  QMainWindow* m_MainWindow;
  BehaviorTreeSceneItem* m_DragItem;
};

#endif
