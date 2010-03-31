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

#include <btree/btree_data.h>

#ifndef STANDARD_RESOURCES_H_INCLUDED
#define STANDARD_RESOURCES_H_INCLUDED

enum NodeStateIcons
{
  ICON_BUG,
  ICON_SUCCESS,
  ICON_FAIL,
  ICON_WORK,
  ICON_COUNT
};

extern const char* const g_NodeSVGResourcePaths[E_MAX_GRIST_TYPES];
extern const char* const g_NodeNames[E_MAX_GRIST_TYPES];
extern const char* const g_IconNames[ICON_COUNT];

#endif /* STANDARD_RESOURCES_H_INCLUDED */
