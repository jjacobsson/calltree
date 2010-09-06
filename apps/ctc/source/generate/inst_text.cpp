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
#include "inst_text.h"

using namespace callback;

const char * const g_CBActionNames[MAXIMUM_NODEACTION_COUNT] =
{
    "CONSTRUCT",
    "EXECUTE",
    "DESTRUCT",
    "PRUNE",
    "MODIFY"
};

const char * const g_NodeReturnsNames[MAXIMUM_NODE_RETURN_COUNT] =
{
    "FAIL",                 /* Node failed                                              */
    "SUCCESS",              /* Node succeded                                            */
    "WORKING",              /* Node continues execution                                 */
    "UNDEFINED"             /* Do not return, used to indicate disabled nodes           */
};

const char * const g_InstructionNames[MAXIMUM_INSTRUCTION_COUNT] =
{
    "INST_CALL_DEBUG_FN",
    "INST_CALL_CONS_FUN",
    "INST_CALL_EXEC_FUN",
    "INST_CALL_DEST_FUN",
    "INST_CALL_PRUN_FUN",
    "INST_CALL_MODI_FUN",
    "INST_JABC_R_EQUA_C",
    "INST_JABC_R_DIFF_C",
    "INST_JABC_C_EQUA_B",
    "INST_JABC_C_DIFF_B",
    "INST_JABB_C_EQUA_B",
    "INST_JABB_C_DIFF_B",
    "INST_JABB_B_EQUA_B",
    "INST_JABB_B_DIFF_B",
    "INST_JABC_CONSTANT",
    "INST_JREC_CONSTANT",
    "INST_JABB_BSSVALUE",
    "INST_JREB_BSSVALUE",
    "INST_JABC_S_C_IN_B",
    "INST_JREC_S_C_IN_B",
    "INST_JABB_S_C_IN_B",
    "INST_JREB_S_C_IN_B",
    "INST__STORE_R_IN_B",
    "INST__STORE_B_IN_R",
    "INST__STORE_C_IN_B",
    "INST__STORE_B_IN_B",
    "INST__STORE_C_IN_R",
    "INST_STORE_PD_IN_B",
    "INST_STORE_PB_IN_R",
    "INST__INC_BSSVALUE",
    "INST__DEC_BSSVALUE",
    "INST__SET_REGISTRY",
    "INST_LOAD_REGISTRY",
    "INST_SCRIPT_C",
    "INST_SCRIPT_R",
    "INST_______SUSPEND"
};
