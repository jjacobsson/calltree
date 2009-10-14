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
#include <btree/btree_data.h>

class BehaviorTreeNode : public QGraphicsSvgItem
{
	Q_OBJECT
public:
	BehaviorTreeNode( NodeGristType type );

protected:
	QVariant itemChange( GraphicsItemChange change, const QVariant &value );
};

#endif
