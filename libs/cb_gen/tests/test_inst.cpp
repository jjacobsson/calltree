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

  const int case_1_b = 0x46;
  const int case_2_b = 0xffff;
  const int case_3_b = 0x103fb;
  const int case_4_b = 0x10400;

  const int case_1   = case_1_b / sizeof( int );
  const int case_2   = case_2_b / sizeof( int );
  const int case_3   = case_3_b / sizeof( int );
  const int case_4   = case_4_b / sizeof( int );

  data[case_1] = 0xcafebabe;
  data[case_2] = 0xcdcdcdcd;
  data[case_3] = 0xdeadbeef;
  data[case_4] = 0xabababab;

  load_with_offset( ilist, er0, eds, case_1 );
  load_with_offset( ilist, er1, eds, case_2 );
  load_with_offset( ilist, er2, eds, case_3 );
  load_with_offset( ilist, er3, eds, case_4 );

  Instruction i;
  i.i  = iexit;
  i.a1 = 0;
  i.a2 = 0;
  i.a3 = 0;
  ilist.push_back( i );

  memcpy( inst, &ilist[0], sizeof(cb::Instruction) * ilist.size() );

  run_program( &cp );

  CHECK( ctx->r[er0] == 0xcafebabe );
  CHECK( ctx->r[er1] == 0xcdcdcdcd );
  CHECK( ctx->r[er2] == 0xdeadbeef );
  CHECK( ctx->r[er3] == 0xabababab );
}


TEST_FIXTURE( VirtualMachineFixture, store_with_offset )
{
  InstList ilist;
  ilist.reserve( 4096 );

  const int case_1_b = 0x46;
  const int case_2_b = 0xffff;
  const int case_3_b = 0x103fb;
  const int case_4_b = 0x10408;

  const int case_1 = (case_1_b / sizeof( int )) - 1;
  const int case_2 = (case_2_b / sizeof( int )) - 1;
  const int case_3 = (case_3_b / sizeof( int )) - 1;
  const int case_4 = (case_4_b / sizeof( int )) - 1;

  data[case_1] = 0xcafebabe;
  data[case_2] = 0xcdcdcdcd;
  data[case_3] = 0xdeadbeef;
  data[case_4] = 0xabababab;

  load_with_offset( ilist, er0, eds, case_1 );
  load_with_offset( ilist, er1, eds, case_2 );
  load_with_offset( ilist, er2, eds, case_3 );
  load_with_offset( ilist, er3, eds, case_4 );

  store_with_offset( ilist, eds, case_1 + 1, er0 );
  store_with_offset( ilist, eds, case_2 + 1, er1 );
  store_with_offset( ilist, eds, case_3 + 1, er2 );
  store_with_offset( ilist, eds, case_4 + 1, er3 );

  Instruction i;
  i.i  = iexit;
  i.a1 = 0;
  i.a2 = 0;
  i.a3 = 0;
  ilist.push_back( i );

  memcpy( inst, &ilist[0], sizeof(cb::Instruction) * ilist.size() );

  run_program( &cp );

  CHECK( data[case_1+1] == 0xcafebabe );
  CHECK( data[case_2+1] == 0xcdcdcdcd );
  CHECK( data[case_3+1] == 0xdeadbeef );
  CHECK( data[case_4+1] == 0xabababab );
}





