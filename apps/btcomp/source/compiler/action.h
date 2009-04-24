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

class FunctionGrist;
class Identifier;

class Action
{
public:
	
	Action()
		: m_Id( 0x0 )
		, m_FunctionId( ~0 )
		, m_Grist( 0x0 )
	{}
	
	Identifier*	m_Id;
	int				m_FunctionId;
	FunctionGrist*	m_Grist;
};

#endif /*ACTION_H_*/
