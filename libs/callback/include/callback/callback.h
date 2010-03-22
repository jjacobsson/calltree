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

#ifndef CALLBACK_PROGRAM_H_
#define CALLBACK_PROGRAM_H_

namespace callback
{

/* A * before an instruction argument means it's a dereference to the bss section           */
/* A $ before an instruction argument means it's a dereference to the data section          */

enum InstructionSet
{
  INST_CALL_DEBUG_FN, /* Makes a debugger call                                    */
  INST_CALL_CONS_FUN, /* Make construction callback                               */
  INST_CALL_EXEC_FUN, /* Make execution callback                                  */
  INST_CALL_DEST_FUN, /* Make destruction callback                                */
  INST_CALL_PRUN_FUN, /* Make prune callback                                      */
  INST_CALL_MODI_FUN, /* Make modify callback                                     */
  INST_JABC_R_EQUA_C, /* Set IP to m_A1 when RE == m_A2                           */
  INST_JABC_R_DIFF_C, /* Set IP to m_A1 when RE != m_A2                           */
  INST_JABC_C_EQUA_B, /* Set IP to m_A1 when m_A2 == *m_A3                        */
  INST_JABC_C_DIFF_B, /* Set IP to m_A1 when m_A2 != *m_A3                        */
  INST_JABB_C_EQUA_B, /* Set IP to *m_A1 when m_A2 == *m_A3                       */
  INST_JABB_C_DIFF_B, /* Set IP to *m_A1 when m_A2 != *m_A3                       */
  INST_JABB_B_EQUA_B, /* Set IP to *m_A1 when *m_A2 == *m_A3                      */
  INST_JABB_B_DIFF_B, /* Set IP to *m_A1 when *m_A2 != *m_A3                      */
  INST_JABC_CONSTANT, /* Set IP to m_A1                                           */
  INST_JREC_CONSTANT, /* Set IP to +m_A1                                          */
  INST_JABB_BSSVALUE, /* Set IP to *m_A1                                          */
  INST_JREB_BSSVALUE, /* Set IP to +*m_A1                                         */
  INST_JABC_S_C_IN_B, /* Set IP to m_A1 and *m_A2 to m_A3                         */
  INST_JREC_S_C_IN_B, /* Set IP to +m_A1 and *m_A2 to m_A3                        */
  INST_JABB_S_C_IN_B, /* Set IP to *m_A1 and *m_A2 to m_A3                        */
  INST_JREB_S_C_IN_B, /* Set IP to +*m_A1 and *m_A2 to m_A3                       */
  INST__STORE_R_IN_B, /* Set *m_A1 to RE                                          */
  INST__STORE_B_IN_R, /* Set RE to *m_A1                                          */
  INST__STORE_C_IN_B, /* Set *m_A1 to m_A2                                        */
  INST__STORE_B_IN_B, /* Set *m_A1 to *m_A2                                       */
  INST__STORE_C_IN_R, /* Set RE to m_A1                                           */
  INST_STORE_PD_IN_B, /* Set B (m_A1) to pointer to D (m_A2)                      */
  INST_STORE_PB_IN_R, /* Set R (m_A1) to pointer to B (m_A2)                      */
  INST__INC_BSSVALUE, /* Set *m_A1 += m_A2                                        */
  INST__DEC_BSSVALUE, /* Set *m_A1 -= m_A2                                        */
  INST_LOAD_REGISTRY, /* Set register m_A1 to the joined value of M_A2 & m_A3     */
  INST_______SUSPEND, /* Halt execution                                           */
  MAXIMUM_INSTRUCTION_COUNT
};

typedef unsigned short VMIType;

struct Instruction
{
  VMIType m_I;
  VMIType m_A1;
  VMIType m_A2;
  VMIType m_A3;
};

struct ProgramHeader
{
  unsigned int m_IC; // Instruction COUNT
  unsigned int m_DS; // Data SIZE
  unsigned int m_BS; // .bss SIZE
};

struct BssHeader
{
  unsigned int m_IC; // Instruction Counter
  unsigned int m_IP; // Instruction Pointer
  unsigned int m_RE; // Return value register
  unsigned int m_R[5]; // Program registers
};

struct CallbackProgram;

enum DebugFlagBits
{
  E_NODE_ACTION   = 0,      // 4 bits
  E_STANDARD_NODE = 1 << 4, // 1 bit
  E_ENTER_SCOPE   = 1 << 5, // 1 bit
  E_EXIT_SCOPE    = 1 << 6, // 1 bit
};

bool act_flag_set( unsigned short, unsigned int );
bool std_flag_set( unsigned short );
bool exi_flag_set( unsigned short );
bool ent_flag_set( unsigned short );

struct DebugInformation
{
  const char* m_Action;
  const char* m_Name;
  unsigned int m_NodeId;
  unsigned short m_Flags;
};

typedef unsigned int (*CallbackHandler)( unsigned int id, unsigned int action, void* bss,
  void** data, void* user_data );
typedef void (*DebugHandler)( CallbackProgram* cp, DebugInformation* di,
  BssHeader*, void* user_data );

struct CallbackProgram
{
  void* m_Program;
  void* m_bss;
  void* m_UserData;
  CallbackHandler m_Callback;
  DebugHandler m_Debug;
};

int run_program( CallbackProgram* info );

}

#endif /* CALLBACK_PROGRAM_H_ */
