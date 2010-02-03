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

#ifndef GRAPHICSITEMTYPES_H_INCLUDED
#define GRAPHICSITEMTYPES_H_INCLUDED

#include <QtGui/QGraphicsItem>

enum CustomGraphicsItemTypes
{
	NodeToNodeArrowType		= QGraphicsItem::UserType + 1,
	BehaviorTreeNodeType
};

#endif /*GRAPHICSITEMTYPES_H_INCLUDED*/
