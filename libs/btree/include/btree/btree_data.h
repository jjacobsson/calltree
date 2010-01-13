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
  E_VART_BOOL,
  E_MAX_VARIABLE_TYPE
};

struct StringData
{
  const char* m_Parsed;
  const char* m_Raw;
};

union VariableData
{
  int        m_Integer;
  float      m_Float;
  StringData m_String;
  bool		 m_Bool;
};

struct Variable
{
  Identifier    m_Id;
  VariableType  m_Type;
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
  E_MAX_GRIST_TYPES
};

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
    NodeGrist       m_Grist;
    Node*			m_Pare;
	Node*			m_Next;
	Node*			m_Prev;
	void*			m_UserData;
    bool			m_Declared;
};

struct BehaviorTree
{
  Identifier    m_Id;
  Node*         m_Root;
  BehaviorTree* m_Next;
  bool          m_Declared;
};

typedef unsigned int mem_size_t;
typedef void* (*AllocateMemoryFunc)( const mem_size_t size );
typedef void (*FreeMemoryFunc)( void* object_pointer );

typedef struct SBehaviorTreeContext* BehaviorTreeContext;

struct BehaviorTreeContextSetup
{
  AllocateMemoryFunc    m_Alloc; // The function that will be used for all memory allocations
  FreeMemoryFunc        m_Free;  // The function that will be used to free all allocated memory
};

enum SymbolTypes
{
  E_ST_TREE,
  E_ST_ACTION,
  E_ST_DECORATOR,
  E_ST_MAX_TYPES
};

union SymbolTypeData
{
  BehaviorTree* m_Tree;
  Action*       m_Action;
  Decorator*    m_Decorator;
};

struct NamedSymbol
{
  SymbolTypes       m_Type;
  SymbolTypeData    m_Symbol;
};

#endif /* BTREE_DATA_H_INCLUDED */
