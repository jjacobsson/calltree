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

class FunctionGrist;
struct Identifier;

class Decorator
{
public:

	Decorator()
		: m_Id( 0x0 )
		, m_FunctionId( ~0 )
		, m_Grist( 0x0 )
		, m_Prune( false )
		, m_Modify( false )
	{}

	Identifier*	m_Id;
	int				m_FunctionId;
	FunctionGrist*	m_Grist;
	bool			m_Prune;
	bool			m_Modify;

};

#endif /*DECORATOR_H_*/
