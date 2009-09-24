/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BTREE_DATA_H_INCLUDED
#define BTREE_DATA_H_INCLUDED

#include <common/types.h>

typedef uint32 hash_t;
const hash_t INVALID_ID = 0xffffffff;

struct Identifier
{
	const char*	m_Text;
	hash_t		m_Hash;
	int			m_Line;
};

enum VariableType
{
    E_VART_UNDEFINED,
    E_VART_INTEGER,
    E_VART_FLOAT,
    E_VART_STRING,
    E_VART_BOOL
};

union VariableData
{
    int         m_Integer;
	float       m_Float;
    const char* m_String;
    bool		m_Bool;
};

struct Variable
{
	VariableType  m_Type;
    Identifier    m_Id;
    VariableData  m_Data;
    Variable*     m_Next;
};


#endif /* BTREE_DATA_H_INCLUDED */
