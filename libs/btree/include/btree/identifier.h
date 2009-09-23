/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef IDENTIFIER_H_
#define IDENTIFIER_H_

#include <common/types.h>

typedef uint32 hash_t;
const hash_t INVALID_ID = 0xffffffff;

struct Identifier
{
	const char*	m_Text;
	hash_t		m_Hash;
	int			m_Line;
};

void InitIdentifier( Identifier* id );

#endif /*IDENTIFIER_H_*/
