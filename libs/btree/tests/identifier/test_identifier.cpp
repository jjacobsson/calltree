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

#include <UnitTest++.h>
#include <btree/btree_data.h>
#include <btree/btree_func.h>

TEST( IdentifierInitSetsAllMembersCorrectly )
{
	Identifier id;
	InitIdentifier( &id );
	CHECK( id.m_Text == 0x0 );
	CHECK( id.m_Hash == INVALID_ID );
	CHECK( id.m_Line == 0 );
}

TEST( IdentifierAssignmentWorksAsExpected )
{
	const char* test_string = "Banan monkey.";
	Identifier i1,i2;
	InitIdentifier( &i1 );
	InitIdentifier( &i2 );
	i1.m_Hash = 0xcafebabe;
	i1.m_Line = 75;
	i1.m_Text = test_string;
	i2 = i1;
	CHECK( i2.m_Hash == 0xcafebabe );
	CHECK( i2.m_Line == 75 );
	CHECK( i2.m_Text == test_string );
}
