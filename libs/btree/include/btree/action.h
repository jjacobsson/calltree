/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef ACTION_H_
#define ACTION_H_

#include "btree_data.h"
#include "btree_func.h"

class FunctionGrist;

class Action
{
public:

	Action()
		: m_FunctionId( ~0 )
		, m_Grist( 0x0 )
	{
		InitIdentifier( &m_Id );
	}
	
	Identifier      m_Id;
	int             m_FunctionId;
	FunctionGrist*  m_Grist;
};

#endif /*ACTION_H_*/
