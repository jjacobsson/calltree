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

#ifndef TEST_VARIABLES_H_INCLUDED
#define TEST_VARIABLES_H_INCLUDED

#include <btree/btree_data.h>
#include <btree/btree_func.h>

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
