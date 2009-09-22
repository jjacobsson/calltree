/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef TEST_VARIABLES_H_INCLUDED
#define TEST_VARIABLES_H_INCLUDED

#include <btree/variable.h>

class TestVariablesFixture
{
public:
	TestVariablesFixture()
	{
		InitVariable( &v );
		InitVariable( &v1 );
		InitVariable( &v2 );
		InitVariable( &v3 );
		InitVariable( &v4 );
	}

	Variable v,v1,v2,v3,v4;
};

#endif
