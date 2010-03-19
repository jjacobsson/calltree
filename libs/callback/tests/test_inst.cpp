/*******************************************************************************
 * Copyright (c) 2010-03-19 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include <UnitTest++.h>
#include "test_inst.h"

TEST_FIXTURE( VirtualMachineFixture, Dummy )
{
  CHECK( true );
}

TEST_FIXTURE( VirtualMachineFixture, ExitInstruction )
{
  inst[0].i = iexit;
  unsigned int r = run_program( &cp );
  CHECK( r == 0 );
}

TEST_FIXTURE( VirtualMachineFixture, InstructionCounter )
{
  inst[0].i = imov;
  inst[0].a1 = er0;
  inst[0].a2 = eic;
  inst[1].i = iexit;
  run_program( &cp );
  CHECK( ctx->r[er0] == 1 );
  CHECK( ctx->r[eic] == 2 );
}

TEST_FIXTURE( VirtualMachineFixture, SomeStackManipulation )
{
  inst[0].i = ipush;
  inst[0].a1 = eic;
  inst[1].i = ipop;
  inst[1].a1 = er1;
  inst[2].i = iexit;
  run_program( &cp );
  CHECK( ctx->r[er1] == 1 );
  CHECK( stack[0] == 1 );
  CHECK( stack[1] == 0 );
  CHECK( ctx->r[eic] == 3 );
}

TEST_FIXTURE( VirtualMachineFixture, isetl )
{
  inst[0].i  = isetl;
  inst[0].a1 = er2;
  inst[0].a2 = 0xca;
  inst[0].a3 = 0xfe;
  inst[1].i  = iexit;
  run_program( &cp );
  CHECK( ctx->r[er2] == 0x0000cafe );
}

TEST_FIXTURE( VirtualMachineFixture, iseth )
{
  inst[0].i  = iseth;
  inst[0].a1 = er2;
  inst[0].a2 = 0xca;
  inst[0].a3 = 0xfe;
  inst[1].i  = iexit;
  run_program( &cp );
  CHECK( ctx->r[er2] == 0xcafe0000 );
}
