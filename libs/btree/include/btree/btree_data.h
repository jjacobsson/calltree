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

#include <btree/btree_func.h> // <- TODO: KILL KILL KILL!

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

enum NodeGristType
{
	E_GRIST_UNKOWN,
	E_GRIST_SEQUENCE,
	E_GRIST_SELECTOR,
	E_GRIST_PARALLEL,
	E_GRIST_DYN_SELECTOR,
	E_GRIST_DECORATOR,
	E_GRIST_ACTION,

	_E_MAX_GRIST_TYPES_
};

class BehaviorTree;

struct Action
{
	Identifier      m_Id;
	Variable*		m_Args;		/* Argument declarations */
	Variable*		m_Vars;		/* Code-generation variables */
	bool			m_Declared;
};

struct Decorator
{
	Identifier      m_Id;
	Variable*		m_Args;		/* Argument declarations */
	Variable*		m_Vars;		/* Code-generation variables */
	bool			m_Declared;
};

struct Node;

struct SequenceGrist
{
	Node* m_FirstChild;
};

struct SelectorGrist
{
	Node* m_FirstChild;
};

struct ParallelGrist
{
	Node* m_FirstChild;
};

struct DynSelectorGrist
{
	Node* m_FirstChild;
};

struct DecoratorGrist
{
	Node*      m_Child;
	Decorator* m_Decorator;
	Variable*  m_Arguments;
};

struct ActionGrist
{
	Action*   m_Action;
	Variable* m_Arguments;
};

struct NodeGrist
{
	NodeGristType    m_Type;
	union
	{
		SequenceGrist    m_Sequence;
		SelectorGrist    m_Selector;
		ParallelGrist    m_Parallel;
		DynSelectorGrist m_DynSelector;
		DecoratorGrist   m_Decorator;
		ActionGrist      m_Action;
	};
};

struct Node
{
    Identifier      m_Id;
    NodeGrist       m_Grist;
	BehaviorTree*   m_Tree;
    Node*			m_Sibling;
    bool			m_Declared;
};

#endif /* BTREE_DATA_H_INCLUDED */
