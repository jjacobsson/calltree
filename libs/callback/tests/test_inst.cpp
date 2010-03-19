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
  inst[0].a1 = er3;
  inst[0].a2 = 0xca;
  inst[0].a3 = 0xfe;
  inst[1].i  = iexit;
  run_program( &cp );
  CHECK( ctx->r[er3] == 0xcafe0000 );
}

TEST_FIXTURE( VirtualMachineFixture, ishftl )
{
  inst[0].i  = isetl;
  inst[0].a1 = er3;
  inst[0].a2 = 0xba;
  inst[0].a3 = 0xbe;
  inst[1].i  = ishftl;
  inst[1].a1 = er2;
  inst[1].a2 = er3;
  inst[1].a3 = 8;
  inst[2].i  = iexit;
  run_program( &cp );
  CHECK( ctx->r[er3] == 0x0000babe );
  CHECK( ctx->r[er2] == 0x00babe00 );
}

TEST_FIXTURE( VirtualMachineFixture, ishftr )
{
  inst[0].i  = iseth;
  inst[0].a1 = er3;
  inst[0].a2 = 0xca;
  inst[0].a3 = 0xfe;
  inst[1].i  = ishftr;
  inst[1].a1 = er2;
  inst[1].a2 = er3;
  inst[1].a3 = 16;
  inst[2].i  = iexit;
  run_program( &cp );
  CHECK( ctx->r[er3] == 0xcafe0000 );
  CHECK( ctx->r[er2] == 0x0000cafe );
}

TEST_FIXTURE( VirtualMachineFixture, icall )
{
  funt[16].m_Start = 5;
  inst[0].i  = isetl;
  inst[0].a1 = er3;
  inst[0].a2 = 0x00;
  inst[0].a3 = 0x10;
  inst[1].i  = icall;
  inst[1].a1 = er3;
  inst[2].i  = iexit;

  inst[5].i  = isetl;
  inst[5].a1 = er0;
  inst[5].a2 = 0xca;
  inst[5].a3 = 0xfe;
  inst[6].i  = ishftl;
  inst[6].a1 = er0;
  inst[6].a2 = er0;
  inst[6].a3 = 16;
  inst[7].i  = iorl;
  inst[7].a1 = er0;
  inst[7].a2 = 0xba;
  inst[7].a3 = 0xbe;
  inst[8].i  = iret;

  run_program( &cp );
  CHECK( ctx->r[er0] == 0xcafebabe );
}






