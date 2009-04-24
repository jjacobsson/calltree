/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef CALLBACK_PROGRAM_H_
#define CALLBACK_PROGRAM_H_

#include <common/types.h>

namespace callback {

    /* A * before an instruction argument means it's a dereference to the bss section           */
    /* A $ before an instruction argument means it's a dereference to the data section          */

    enum InstructionSet
    {
        INST_CALL_DEBUG_FN,         /* Makes a debugger call                                    */
        INST_CALL_CONS_FUN,         /* Make construction callback                               */
        INST_CALL_EXEC_FUN,         /* Make execution callback                                  */
        INST_CALL_DEST_FUN,         /* Make destruction callback                                */
        INST_CALL_PRUN_FUN,         /* Make prune callback                                      */
        INST_CALL_MODI_FUN,         /* Make modify callback                                     */
        INST_JABC_R_EQUA_C,         /* Set IP to m_A1 when RE == m_A2                           */
        INST_JABC_R_DIFF_C,         /* Set IP to m_A1 when RE != m_A2                           */
        INST_JABC_C_EQUA_B,         /* Set IP to m_A1 when m_A2 == *m_A3                        */
        INST_JABC_C_DIFF_B,         /* Set IP to m_A1 when m_A2 != *m_A3                        */
        INST_JABB_C_EQUA_B,         /* Set IP to *m_A1 when m_A2 == *m_A3                       */
        INST_JABB_C_DIFF_B,         /* Set IP to *m_A1 when m_A2 != *m_A3                       */
        INST_JABB_B_EQUA_B,         /* Set IP to *m_A1 when *m_A2 == *m_A3                      */
        INST_JABB_B_DIFF_B,         /* Set IP to *m_A1 when *m_A2 != *m_A3                      */
        INST_JABC_CONSTANT,         /* Set IP to m_A1                                           */
        INST_JREC_CONSTANT,         /* Set IP to +m_A1                                          */
        INST_JABB_BSSVALUE,         /* Set IP to *m_A1                                          */
        INST_JREB_BSSVALUE,         /* Set IP to +*m_A1                                         */
        INST_JABC_S_C_IN_B,         /* Set IP to m_A1 and *m_A2 to m_A3                         */
        INST_JREC_S_C_IN_B,         /* Set IP to +m_A1 and *m_A2 to m_A3                        */
        INST_JABB_S_C_IN_B,         /* Set IP to *m_A1 and *m_A2 to m_A3                        */
        INST_JREB_S_C_IN_B,         /* Set IP to +*m_A1 and *m_A2 to m_A3                       */
        INST__STORE_R_IN_B,         /* Set *m_A1 to RE                                          */
        INST__STORE_B_IN_R,         /* Set RE to *m_A1                                          */
        INST__STORE_C_IN_B,         /* Set *m_A1 to m_A2                                        */
        INST__STORE_B_IN_B,         /* Set *m_A1 to *m_A2                                       */
        INST__STORE_C_IN_R,         /* Set RE to m_A1                                           */
        INST_STORE_PD_IN_B,         /* Set B (m_A1) to pointer to D (m_A2)                      */
        INST_STORE_PB_IN_R,         /* Set R (m_A1) to pointer to B (m_A2)                      */
        INST__INC_BSSVALUE,         /* Set *m_A1 += m_A2                                        */
        INST__DEC_BSSVALUE,         /* Set *m_A1 -= m_A2                                        */
        INST_LOAD_REGISTRY,         /* Set register m_A1 to the joined value of M_A2 & m_A3     */
        INST_______SUSPEND,         /* Halt execution                                           */
        MAXIMUM_INSTRUCTION_COUNT
    };

    typedef uint16 VMIType;

    struct Instruction
    {
        VMIType m_I;
        VMIType m_A1;
        VMIType m_A2;
        VMIType m_A3;
    };

    struct ProgramHeader
    {
        uint32 m_IC;   // Instruction COUNT
        uint32 m_DS;   // Data SIZE
        uint32 m_BS;   // .bss SIZE
    };

    struct BssHeader
    {
        uint32 m_IC;   // Instruction Counter
        uint32 m_IP;   // Instruction Pointer
        uint32 m_RE;   // Return value register
        uint32 m_R[5]; // Program Registries
    };

    struct CallbackProgram;

    typedef uint32 (*callbackHandler)( uint32 id, uint32 action, void* bss, void** data, void* user_data );
    typedef void (*DebugHandler)( const char* info, CallbackProgram* cbp, ProgramHeader ph, BssHeader* bh );

    struct CallbackProgram
    {
        void*           m_Program;
        void*           m_bss;
        void*           m_UserData;
        callbackHandler m_callback;
        DebugHandler    m_Debug;
    };

    int run_program( CallbackProgram* info );

}

#endif /* CALLBACK_PROGRAM_H_ */
