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
    "INST_CALL_DEBUG_FN",           /* Makes a debugger call                                    */
    "INST_CALL_CONS_FUN",           /* Make construciton callback, R0, R1 and R2 used           */
    "INST_CALL_EXEC_FUN",           /* Make execution callback, R0, R1 and R2 used              */
    "INST_CALL_DEST_FUN",           /* Make destruction callback, R0, R1 and R2 used            */
    "INST_CALL_PRUN_FUN",           /* Make prune callback, R0, R1 and R2 used                  */
    "INST_CALL_MODI_FUN",           /* Make modify callback, R0, R1 and R2 used                 */
    "INST_JABC_R_EQUA_C",           /* Set IP to m_A1 when RE == m_A2                           */
    "INST_JABC_R_DIFF_C",           /* Set IP to m_A1 when RE != m_A2                           */
    "INST_JABC_C_EQUA_B",           /* Set IP to m_A1 when m_A2 == *m_A3                        */
    "INST_JABC_C_DIFF_B",           /* Set IP to m_A1 when m_A2 != *m_A3                        */
    "INST_JABB_C_EQUA_B",           /* Set IP to *m_A1 when m_A2 == *m_A3                       */
    "INST_JABB_C_DIFF_B",           /* Set IP to *m_A1 when m_A2 != *m_A3                       */
    "INST_JABB_B_EQUA_B",           /* Set IP to *m_A1 when *m_A2 == *m_A3                      */
    "INST_JABB_B_DIFF_B",           /* Set IP to *m_A1 when *m_A2 != *m_A3                      */
    "INST_JABC_CONSTANT",           /* Set IP to m_A1                                           */
    "INST_JREC_CONSTANT",           /* Set IP to +m_A1                                          */
    "INST_JABB_BSSVALUE",           /* Set IP to *m_A1                                          */
    "INST_JREB_BSSVALUE",           /* Set IP to +*m_A1                                         */
    "INST_JABC_S_C_IN_B",           /* Set IP to m_A1 and *m_A2 to m_A3                         */
    "INST_JREC_S_C_IN_B",           /* Set IP to +m_A1 and *m_A2 to m_A3                        */
    "INST_JABB_S_C_IN_B",           /* Set IP to *m_A1 and *m_A2 to m_A3                        */
    "INST_JREB_S_C_IN_B",           /* Set IP to +*m_A1 and *m_A2 to m_A3                       */
    "INST__STORE_R_IN_B",           /* Set *m_A1 to RE                                          */
    "INST__STORE_B_IN_R",           /* Set RE to *m_A1                                          */
    "INST__STORE_C_IN_B",           /* Set *m_A1 to m_A2                                        */
    "INST__STORE_B_IN_B",           /* Set *m_A1 to *m_A2                                       */
    "INST__STORE_C_IN_R",           /* Set RE to m_A1                                           */
    "INST_STORE_PD_IN_B",           /* Set B (m_A1) to pointer to D (m_A2)                      */
    "INST_STORE_PB_IN_R",           /* Set R (m_A1) to pointer to B (m_A2)                      */
    "INST__INC_BSSVALUE",           /* Set *m_A1 += m_A2                                        */
    "INST__DEC_BSSVALUE",           /* Set *m_A1 -= m_A2                                        */
    "INST_LOAD_REGISTRY",           /* Set registry m_A1 to the joined value of M_A2 & m_A3     */
    "INST_______SUSPEND"            /* Halt execution                                           */
};
