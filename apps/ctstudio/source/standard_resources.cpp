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

#include "standard_resources.h"

const char* const g_NodeSVGResourcePaths[E_MAX_GRIST_TYPES] = {
    ":/nodes/unknown.svg",
    ":/nodes/sequence.svg",
    ":/nodes/selector.svg",
    ":/nodes/parallel.svg",
    ":/nodes/dyn_selector.svg",
    ":/nodes/succeed.svg",
    ":/nodes/fail.svg",
    ":/nodes/work.svg",
    ":/nodes/leaf.svg",
    ":/nodes/action.svg",
    ":/nodes/decorator.svg"
};

const char* const g_NodeIcons[E_MAX_GRIST_TYPES] = {
    ":/icons/unknown.png",
    ":/icons/sequence.png",
    ":/icons/selector.png",
    ":/icons/parallel.png",
    ":/icons/dyn_selector.png",
    ":/icons/succeed.png",
    ":/icons/fail.png",
    ":/icons/work.png",
    ":/icons/leaf.png",
    ":/icons/action.png",
    ":/icons/decorator.png"
};


const char* const g_NodeNames[E_MAX_GRIST_TYPES] = {
  "Unknown",
  "Sequence",
  "Selector",
  "Parallel",
  "Dynamic Selector",
  "Succeed",
  "Fail",
  "Work",
  "Tree",
  "Action",
  "Decorator"
};
