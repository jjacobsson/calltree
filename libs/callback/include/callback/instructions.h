/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef CALLBACK_INSTRUCTIONS_H_
#define CALLBACK_INSTRUCTIONS_H_

#include <common/types.h>

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
        E_NODE_RUNNING,                 /* Node continues execution                             */
        E_NODE_UNDEFINED,               /* Do not return, used to indicate disabled nodes       */
        MAXIMUM_NODE_RETURN_COUNT
    };

}

#endif /*CALLBACK_INSTRUCTIONS_H_*/
