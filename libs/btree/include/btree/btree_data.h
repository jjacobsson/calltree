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

#ifndef BTREE_DATA_H_INCLUDED
#define BTREE_DATA_H_INCLUDED

typedef unsigned int hash_t;
const hash_t INVALID_ID = 0xffffffff;

enum NodeGristType
{
  E_GRIST_UNKOWN,
  E_GRIST_SEQUENCE,
  E_GRIST_SELECTOR,
  E_GRIST_PARALLEL,
  E_GRIST_DYN_SELECTOR,
  E_GRIST_SUCCEED,
  E_GRIST_FAIL,
  E_GRIST_WORK,
  E_GRIST_DECORATOR,
  E_GRIST_ACTION,
  E_MAX_GRIST_TYPES
};

enum SymbolTypes
{
  E_ST_UNKOWN,
  E_ST_TREE,
  E_ST_NODE,
  E_ST_ACTION,
  E_ST_DECORATOR,
  E_ST_MAX_TYPES
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

struct Identifier
{
	const char*	m_Text;
	hash_t		m_Hash;
	int    		m_Line;
};

struct StringData
{
  const char* m_Parsed;
  const char* m_Raw;
};

union VariableData
{
  int      m_Integer;
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
struct BehaviorTree;

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

struct NodeParent
{
  SymbolTypes m_Type;
  union {
    Node*           m_Node;
    BehaviorTree*   m_Tree;
  };
};

struct Node
{
    NodeGrist       m_Grist;
    NodeParent		m_Pare;
	Node*			m_Next;
	Node*			m_Prev;
	void*			m_UserData;
};

struct BehaviorTree
{
  Identifier    m_Id;
  Node*         m_Root;
  void*         m_UserData;
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

struct Include
{
  hash_t        m_Hash;
  const char*   m_Name;
  const char*   m_Parent;
  int           m_LineNo;
  Include*      m_Next;
};

struct StringBuffer
{
  char* m_Str;
  int   m_Size;
  int   m_Capacity;
};

typedef struct SParserContext* ParserContext;
typedef struct SBehaviorTreeContext* BehaviorTreeContext;

typedef void (*ParserErrorFunction)( ParserContext, const char* msg );
typedef void (*ParserWarningFunction)( ParserContext, const char* msg );
typedef int (*ParserFillBufferFunction)( ParserContext, char* buffer, int maxsize );
typedef const char* (*ParserTranslateIncludeFunction)( ParserContext, const char* );

struct ParserContextFunctions
{
  ParserErrorFunction               m_Error;
  ParserWarningFunction             m_Warning;
  ParserFillBufferFunction          m_Read;
  ParserTranslateIncludeFunction    m_Translate;
};

#endif /* BTREE_DATA_H_INCLUDED */
