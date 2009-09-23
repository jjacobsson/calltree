/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef DECORATOR_H_
#define DECORATOR_H_

#include "identifier.h"

class FunctionGrist;

class Decorator
{
public:

	Decorator()
		: m_FunctionId( ~0 )
		, m_Grist( 0x0 )
		, m_Prune( false )
		, m_Modify( false )
	{
		InitIdentifier( &m_Id );
	}

	Identifier      m_Id;
	int				m_FunctionId;
	FunctionGrist*	m_Grist;
	bool			m_Prune;
	bool			m_Modify;

};

#endif /*DECORATOR_H_*/
