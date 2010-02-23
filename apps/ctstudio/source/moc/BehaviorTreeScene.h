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

// BT Forward declares
struct Node;
struct BehaviorTree;
class BehaviorTreeSceneItem;
typedef struct SBehaviorTreeContext* BehaviorTreeContext;

class BehaviorTreeScene : public QGraphicsScene
{
	Q_OBJECT
public:
	BehaviorTreeScene( QMainWindow* );
	~BehaviorTreeScene();

	bool readFile( const QString& fileName );
	bool writeFile( const QString& fileName );

    void dragEnterEvent( QDragEnterEvent *event );
    void dragLeaveEvent( QDragLeaveEvent *event );
    void dragMoveEvent( QDragMoveEvent *event );
    void dropEvent( QDropEvent *event );

signals:
  void modified();

public slots:
	void layout();
	void itemModified();

protected:

    struct Extents
    {
    	Extents()
			: l( 0.0f )
			, r( 0.0f )
		{}
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
    void mergeExtents( ExtentsList& res, const ExtentsList& lel, const ExtentsList& rel );
    void padExtents( ExtentsList& l, const ExtentsList& r );

    void drawItems(
		QPainter* painter,
		int numItems,
		QGraphicsItem* items[],
		const QStyleOptionGraphicsItem options[],
		QWidget* widget = 0
    );

	QGraphicsView*  	m_View;
	QGraphicsScene* 	m_Scene;
	BehaviorTreeContext	m_TreeContext;
	QMainWindow*        m_MainWindow;
};

#endif
