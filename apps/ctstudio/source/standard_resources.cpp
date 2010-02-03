/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "standard_resources.h"

const char* const g_NodeSVGResourcePaths[E_MAX_GRIST_TYPES] = {
    ":/nodes/unknown.svg",
    ":/nodes/sequence.svg",
    ":/nodes/selector.svg",
    ":/nodes/parallel.svg",
    ":/nodes/dyn_selector.svg",
    ":/nodes/decorator.svg",
    ":/nodes/action.svg"
};

const char* const g_NodeNames[E_MAX_GRIST_TYPES] = {
  "Unknown",
  "Sequence",
  "Selector",
  "Parallel",
  "Dynamic Selector",
  "Decorator",
  "Action"
};
