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
  inop,          /* No-Op */
  icall,         /* push ip+1 to sp and set ip to toc[a1] */
  iret,          /* pop sp to ip */
  iccall,        /* */
  idcall,        /* */
  ibr,           /* */
  ibre,          /* */
  ibrne,         /* */
  imov,          /* */
  iadd,          /* */
  isub,          /* */
  imul,          /* */
  idiv,          /* */
  iinc,          /* */
  idec,          /* */
  iload,         /* */
  istore,        /* */
  ipush,         /* */
  ipop,          /* */
  ishftl,        /* */
  ishftr,        /* */
  iand,          /* */
  ior,           /* */
  ixor,          /* */
  isetl,         /* */
  iseth,         /* */
  iandl,         /* */
  iandh,         /* */
  iorl,          /* */
  iorh,          /* */
  ixorl,         /* */
  ixorh,         /* */
  iexit,         /* */
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
  er1,   /* 02 General purpose */
  er2,   /* 03 General purpose */
  er3,   /* 04 General purpose */
  er4,   /* 05 General purpose */
  er5,   /* 06 General purpose */
  er6,   /* 07 General purpose */
  er7,   /* 08 General purpose */
  er8,   /* 09 General purpose */
  er9,   /* 10 General purpose */
  er10,  /* 11 General purpose */
  er11,  /* 12 General purpose */
  er12,  /* 13 General purpose */
  er13,  /* 14 General purpose */
  er14,  /* 15 General purpose */
  er15,  /* 16 General purpose */
  er16,  /* 17 General purpose */
  er17,  /* 18 General purpose */
  er18,  /* 19 General purpose */
  er19,  /* 20 General purpose */
  er20,  /* 21 General purpose */
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
