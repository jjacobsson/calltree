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
#include "../source/gen_utility.h"

#include <string.h>

using namespace cb_gen;

TEST_FIXTURE( VirtualMachineFixture, load_with_offset )
{
  InstList ilist;
  ilist.reserve( 4096 );

  data[64]    = 0xcafebabe;
  data[786]   = 0xdeadbeef;
  data[272]   = 0xcdcdcdcd;
  data[66500] = 0xabababab;
  data[70000] = 0xfefefefe;

  load_with_offset( ilist, er0, eds, 64 );
  load_with_offset( ilist, er1, eds, 786 );
  load_with_offset( ilist, er2, eds, 272 );
  load_with_offset( ilist, er3, eds, 66500 );
  load_with_offset( ilist, er4, eds, 70000 );

  Instruction i;
  i.i  = iexit;
  i.a1 = 0;
  i.a2 = 0;
  i.a3 = 0;
  ilist.push_back( i );

  memcpy( inst, &ilist[0], sizeof(cb::Instruction) * ilist.size() );

  run_program( &cp );

  CHECK( ctx->r[er0] == 0xcafebabe );
  CHECK( ctx->r[er1] == 0xdeadbeef );
  CHECK( ctx->r[er2] == 0xcdcdcdcd );
  CHECK( ctx->r[er3] == 0xabababab );
  CHECK( ctx->r[er4] == 0xfefefefe );
}






