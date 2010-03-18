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
  "noop",
  "call",
  "ret",
  "ccall",
  "dcall",
  "mov",
  "load",
  "store",
  "push",
  "pop",
  "exit"
};
