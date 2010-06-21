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

const char * const g_CBActionNames[cb::MAXIMUM_NODEACTION_COUNT] =
{ "CONSTRUCT", "EXECUTE", "DESTRUCT", "PRUNE", "MODIFY" };

const char * const g_NodeReturnsNames[cb::MAXIMUM_NODE_RETURN_COUNT] =
{
  "FAIL",     /* Node failed                                              */
  "SUCCESS",  /* Node succeded                                            */
  "WORKING",  /* Node continues execution                                 */
  "UNDEFINED" /* Do not return, used to indicate disabled nodes           */
};

const char * const g_InstructionNames[cb::inst_count] =
{
  "nop",          /* 0 No-Op */
  "call",         /* 1 */
  "ret",          /* 2 */
  "ccall",        /* 3 */
  "dcall",        /* 4 */
  "br",           /* 5 */
  "bre",          /* 6 */
  "brne",         /* 7 */
  "mov",          /* 8 */
  "add",          /* 9 */
  "sub",          /* 10 */
  "mul",          /* 11 */
  "div",          /* 12 */
  "addi",         /* 13 */
  "subi",         /* 14 */
  "muli",         /* 15 */
  "divi",         /* 16 */
  "ld",           /* 17 */
  "st",           /* 18 */
  "push",         /* 19 */
  "pop",          /* 20 */
  "shli",         /* 21 */
  "shri",         /* 22 */
  "and",          /* 23 */
  "or",           /* 24 */
  "xor",          /* 25 */
  "slszi",        /* 26 */
  "shszi",        /* 27 */
  "andl",         /* 28 */
  "andh",         /* 29 */
  "orl",          /* 30 */
  "orh",          /* 31 */
  "xorl",         /* 32 */
  "xorh",         /* 33 */
  "exit"          /* 34 */
};
