/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BEHAVOIRTREENODE_H_INCLUDED
#define BEHAVOIRTREENODE_H_INCLUDED

#include <QtSvg/QGraphicsSvgItem>
#include <QtCore/QList>

// BT Forward declares
struct Node;

// Internal Forward declares
class NodeToNodeArrow;

class BehaviorTreeNode : public QGraphicsSvgItem
{
	Q_OBJECT
public:
	BehaviorTreeNode( Node* n, BehaviorTreeNode* parent = 0x0 );

    void removeArrow( NodeToNodeArrow* arrow );
    void removeArrows();
    void addArrow( NodeToNodeArrow* arrow );

protected:
	QVariant itemChange( GraphicsItemChange change, const QVariant &value );

	void mousePressEvent( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );

private:
	Node* m_Node;

	QList<NodeToNodeArrow*> m_Arrows;

	bool m_LeftMouseDown;
};

#endif
