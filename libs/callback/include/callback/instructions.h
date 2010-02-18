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

#ifndef CALLBACK_INSTRUCTIONS_H_
#define CALLBACK_INSTRUCTIONS_H_

#include <callback/types.h>

namespace callback {

    enum NodeAction
    {
        ACT_CONSTRUCT,              /* Tell callback to do setup                                */
        ACT_EXECUTE,                /* Tell callback to execute it's meat                       */
        ACT_DESTRUCT,               /* Tell callback to do tear-down                            */
        ACT_PRUNE,                  /* Tell the decorator to act as a branch pruner             */
        ACT_MODIFY,                 /* Allow the decorator to modify the child return value     */
        MAXIMUM_NODEACTION_COUNT
    };

    enum NodeReturns
    {
        E_NODE_FAIL,                    /* Node failed                                          */
        E_NODE_SUCCESS,                 /* Node succeeded                                       */
        E_NODE_WORKING,                 /* Node continues execution                             */
        E_NODE_UNDEFINED,               /* Do not return, used to indicate disabled nodes       */
        MAXIMUM_NODE_RETURN_COUNT
    };

}

#endif /*CALLBACK_INSTRUCTIONS_H_*/
