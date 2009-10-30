/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BEHAVOIRTREESCENE_H_INCLUDED
#define BEHAVOIRTREESCENE_H_INCLUDED

#include <QtGui/QGraphicsScene>

#include <vector>

// Qt Forward declares
class QString;
class QGraphicsItem;

// BT Forward declares
struct Node;
class BehaviorTree;
class BehaviorTreeNode;

class BehaviorTreeScene : public QGraphicsScene
{
	Q_OBJECT
public:
	BehaviorTreeScene();
	~BehaviorTreeScene();

	bool readFile( const QString& fileName );

public slots:
	void layoutNodes();

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

	void createGraphics( Node*, BehaviorTreeNode* );
	void layoutNode( Node* );
	void depthFirstPlace( Node* n, ExtentsList& pel );
    double minimumRootDistance( const ExtentsList& l, const ExtentsList& r );
    void moveExtents( ExtentsList& el, double dist );
    void mergeExtents( ExtentsList& res, const ExtentsList& lel, const ExtentsList& rel );
    void transformToWorld( Node* n, Node* p );

	QGraphicsView*  m_View;
	QGraphicsScene* m_Scene;
	BehaviorTree*	m_Tree;
};

#endif
