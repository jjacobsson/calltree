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

namespace cb
{

  enum NodeAction
  {
    ACT_CONSTRUCT, /* Tell callback to do setup                                */
    ACT_EXECUTE,   /* Tell callback to execute it's meat                       */
    ACT_DESTRUCT,  /* Tell callback to do tear-down                            */
    ACT_PRUNE,     /* Tell the decorator to act as a branch pruner             */
    ACT_MODIFY,    /* Allow the decorator to modify the child return value     */
    MAXIMUM_NODEACTION_COUNT
  };

  enum NodeReturns
  {
    E_NODE_FAIL,      /* Node failed                                          */
    E_NODE_SUCCESS,   /* Node succeeded                                       */
    E_NODE_WORKING,   /* Node continues execution                             */
    E_NODE_UNDEFINED, /* Do not return, used to indicate disabled nodes       */
    MAXIMUM_NODE_RETURN_COUNT
  };

  enum InstructionSet
  {
    inop,          /* 0 No-Op */
    icall,         /* 1 */
    iret,          /* 2 */
    iccall,        /* 3 */
    idcall,        /* 4 */
    ibr,           /* 5 */
    ibre,          /* 6 */
    ibrne,         /* 7 */
    imov,          /* 8 */
    iadd,          /* 9 */
    isub,          /* 10 */
    imul,          /* 11 */
    idiv,          /* 12 */
    iaddi,         /* 13 */
    isubi,         /* 14 */
    imuli,         /* 15 */
    idivi,         /* 16 */
    ild,           /* 17 */
    ist,           /* 18 */
    ipush,         /* 19 */
    ipop,          /* 20 */
    ishli,         /* 21 */
    ishri,         /* 22 */
    iand,          /* 23 */
    ior,           /* 24 */
    ixor,          /* 25 */
    islszi,        /* 26 */
    ishszi,        /* 27 */
    iandl,         /* 28 */
    iandh,         /* 29 */
    iorl,          /* 30 */
    iorh,          /* 31 */
    ixorl,         /* 32 */
    ixorh,         /* 33 */
    iexit,         /* 34 */
    inst_count
  };

  struct Instruction
  {
    unsigned char i;
    unsigned char a1;
    unsigned char a2;
    unsigned char a3;
  };

  enum Register
  {
    er0,   /* 00 General purpose */
    er1,   /* 01 General purpose */
    er2,   /* 02 General purpose */
    er3,   /* 03 General purpose */
    er4,   /* 04 General purpose */
    er5,   /* 05 General purpose */
    er6,   /* 06 General purpose */
    er7,   /* 07 General purpose */
    er8,   /* 08 General purpose */
    er9,   /* 09 General purpose */
    er10,  /* 10 General purpose */
    er11,  /* 11 General purpose */
    er12,  /* 12 General purpose */
    er13,  /* 13 General purpose */
    er14,  /* 14 General purpose */
    er15,  /* 15 General purpose */
    er16,  /* 16 General purpose */
    er17,  /* 17 General purpose */
    er18,  /* 18 General purpose */
    er19,  /* 19 General purpose */
    er20,  /* 20 General purpose */
    er21,  /* 21 General purpose */
    gen_reg_count,
    err = gen_reg_count,   /* 22 Return value register */
    efc,   /* 23 Frame counter */
    eic,   /* 24 Instruction counter */
    eip,   /* 25 Instruction pointer */
    esp,   /* 26 Stack pointer */
    efs,   /* 27 Call stack pointer */
    eds,   /* 28 Data section */
    ems,   /* 29 Memory section */
    eft,   /* 30 Function Lookup Table */
    ejt,   /* 31 Jump target lookup table */
    reg_count
  };

  struct Context
  {
    unsigned int r[reg_count]; // Program registers
  };

  struct ProgramHeader
  {
    unsigned int m_Inst;   // Offset to the first Instruction (i.e. sizeof( ProgramHeader ))
    unsigned int m_Data;   // Offset to the first data byte
    unsigned int m_Funt;   // Offset to the first FunctionTableEntry
    unsigned int m_Jump;   // Offset to the first jump target
    unsigned int m_Bss;    // Offset from memory to the start of the bss (i.e. sizeof( Context ))
    unsigned int m_Stack;  // Offset from memory to the start of the stack
    unsigned int m_CStack; // Offset from memory to the start of the call stack
  };

  struct FunctionTableEntry
  {
    unsigned int m_Id;    /* Unique identifier */
    unsigned int m_Start; /* Instruction index where the function starts */
  };

  struct CallFrame
  {
    char* m_Bss;
    int   m_IP;
  };

  struct CallbackProgram;

  enum DebugFlagBits
  {
    E_NODE_ACTION    = 0,      // 4 bits
    E_STANDARD_NODE  = 1 << 4, // 1 bit
    E_COMPOSITE_NODE = 1 << 5, // 1 bit
    E_ENTER_SCOPE    = 1 << 6, // 1 bit
    E_EXIT_SCOPE     = 1 << 7  // 1 bit
  };

  bool act_flag_set( unsigned int, unsigned int );
  bool std_flag_set( unsigned int );
  bool com_flag_set( unsigned int );
  bool exi_flag_set( unsigned int );
  bool ent_flag_set( unsigned int );

  struct DebugData
  {
    const char* m_Action;
    const char* m_Name;
    unsigned int m_NodeId;
    unsigned int m_Flags;
    unsigned int m_LineNo;
  };

  struct CallData
  {
    unsigned int  m_Id;
    unsigned int  m_Action;
    void*         m_Memory;
    void**        m_Parameters;
  };

  typedef unsigned int (*CallbackHandler)( CallData*, void* );
  typedef void (*DebugHandler)( CallbackProgram*, DebugData*, Context* );

  struct CallbackProgram
  {
    void* m_Program;
    void* m_Memory;
    void* m_UserData;
    CallbackHandler m_Callback;
    DebugHandler m_Debug;
  };

  int run_program( CallbackProgram* info );

}

#endif /* CALLBACK_PROGRAM_H_ */
