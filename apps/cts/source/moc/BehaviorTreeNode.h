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

#include "../GraphicsItemTypes.h"

// BT Forward declares
struct Node;

// Internal Forward declares
class NodeToNodeArrow;

class BehaviorTreeNode : public QGraphicsSvgItem
{
	Q_OBJECT
public:

	enum
	{
		Type = BehaviourTreeNodeType
	};

	BehaviorTreeNode( Node* n, BehaviorTreeNode* parent = 0x0 );

	int type() const
	{
		return Type;
	}

    void removeArrow( NodeToNodeArrow* arrow );
    void removeArrows();
    void addArrow( NodeToNodeArrow* arrow );

    NodeToNodeArrow* findArrowTo( BehaviorTreeNode* other );

signals:

	void nodeDragged();

protected:
	QVariant itemChange( GraphicsItemChange change, const QVariant &value );

	void mousePressEvent( QGraphicsSceneMouseEvent* event );
	void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
	void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

	void draggingStarted();
	void draggingEnded();

private:

	void setupRelinkage();
	void executeRelinkage();

	enum MouseState
	{
		E_MS_NONE,
		E_MS_LB_DOWN,
		E_MS_DRAGGING
	};

	QList<NodeToNodeArrow*> m_Arrows;

	Node*				m_Node;				// Associated behavior tree node.
	MouseState			m_MouseState;		// Current state of mouse handling.
	QPoint				m_StartPos;			// Item's screen position when the LB was pressed;

	struct Relinkage
	{
		Node*	m_Parent;			// The new parent node of this item
		Node*	m_Sibling;			// The new sibling node of this item
		bool	m_BeforeSibling;	// If this is true this node should be linked in before the sibling, otherwise it should be linked after the sibling.
	};

	Relinkage			m_Relinkage;		// This is the information needed to be able to correctly link the m_Node into a new position in the BT.
	NodeToNodeArrow*	m_DraggingArrow;	// This arrow is the one used to indicate where the node will be linked while dragging.

};

#endif
