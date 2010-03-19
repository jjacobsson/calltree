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
  imov,          /* */
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
  er0,
  er1,
  er2,
  er3,
  err,           /* Return value register */
  efc,           /* Frame counter */
  eic,           /* Instruction counter */
  eip,           /* Instruction pointer */
  esp,           /* Stack pointer */
  efs,           /* Call stack pointer */
  eds,           /* Data section */
  ems,           /* Memory section */
  eft,           /* Function Lookup Table */
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
  unsigned int m_Bss;    // Offset from memory to the start of the bss (i.e. sizeof( Context ))
  unsigned int m_Stack;  // Offset from memory to the start of the stack
  unsigned int m_CStack; // Offset from memory to the start of the call stack
};

struct FunctionTableEntry
{
  unsigned int m_Id;    /* Unique identifier */
  unsigned int m_Start; /* Instruction index where the function starts */
};

struct CallbackProgram;

union DebugFlags
{
  unsigned int   m_Flags;
  struct
  {
    unsigned char  m_Exit:1;
    unsigned char  m_Standard:1;
    unsigned char  m_Action:4;
  };
};

struct CallData
{
  unsigned int  m_Id;
  unsigned int  m_Action;
  void*         m_Memory;
  void**        m_Parameters;
};

struct DebugData
{
  const char* m_Action;
  const char* m_Name;
  unsigned int m_NodeId;
  unsigned int m_Flags;
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
