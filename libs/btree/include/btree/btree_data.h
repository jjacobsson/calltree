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
	E_GRIST_SEQUENCE,
	E_GRIST_SELECTOR,
	E_GRIST_PARALLEL,
	E_GRIST_DYN_SELECTOR,
	E_GRIST_DECORATOR,
	E_GRIST_ACTION,

	_E_MAX_GRIST_TYPES_
};

class BehaviourTree;

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
class NodeGrist;

class NodeGrist
{
public:
	NodeGrist();
	virtual ~NodeGrist();

	virtual int GetType() const = 0;

	virtual void GenerateConstructionCode( BehaviourTree* bt ) {};
	virtual void GenerateExecutionCode( BehaviourTree* bt ) {};
	virtual void GenerateDestructionCode( BehaviourTree* bt ) {};

	virtual void SetChaff( Node* chaff )
	{
		m_Chaff = chaff;
	}

	static int s_GristCount;

	Node* m_Chaff;
};

/*
 * I can't believe I wrote this class...
 */

class NodeList
{
public:

	NodeList()
		: m_Count( 0 )
	{}

	static const int s_MaxNodeCount = 128;

	typedef Node** iterator;
	typedef const Node* const* const_iterator;

	inline iterator begin()
	{
		return &m_Nodes[0];
	}

	inline iterator end()
	{
		return &m_Nodes[m_Count];
	}

	inline const_iterator begin() const
	{
		return &m_Nodes[0];
	}
	inline const_iterator end() const
	{
		return &m_Nodes[m_Count];
	}

	inline Node* operator[] (int i)
	{
		return m_Nodes[i];
	}

	inline const Node* operator[] (int i) const
	{
		return m_Nodes[i];
	}

	void SetParent( Node* node );
	bool Append( Node* node );
	int Size();

private:
	Node*	m_Nodes[ s_MaxNodeCount ];
	int		m_Count;
};

#include <vector> // <- TODO: KILL KILL KILL!

class VariableOwner : public NodeGrist
{
public:

    VariableOwner();
    ~VariableOwner();

    void SetVariableList( Variable* variables );
    Variable* GetVariableList();

protected:

    void StoreVariablesInDataSection( BehaviourTree* bt, Variable* declaration );
    void GenerateVariableInstructions( BehaviourTree* bt );
    void SetupVariableRegistry( BehaviourTree* bt );

    Variable*			m_Variables;
    int                 m_bssStart;
    std::vector<int>    m_Data; // <- TODO: KILL KILL KILL!
};

class SequenceNodeGrist : public NodeGrist
{
public:
    SequenceNodeGrist();
    ~SequenceNodeGrist();

    int GetType() const { return E_GRIST_SEQUENCE; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void SetChaff( Node* n );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:

    NodeList*  m_Children;
    int        m_bss_ReEntry;
    int        m_bss_JumpBackTarget;
};

class SelectorNodeGrist : public NodeGrist
{
public:
    SelectorNodeGrist();
    ~SelectorNodeGrist();

    int GetType() const { return E_GRIST_SELECTOR; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void SetChaff( Node* n );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:
    NodeList*  m_Children;
    int        m_bss_ReEntry;
    int        m_bss_JumpBackTarget;
};

class ParallelNodeGrist : public NodeGrist
{
public:
    ParallelNodeGrist(  );
    ~ParallelNodeGrist();

    int GetType() const { return E_GRIST_PARALLEL; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void SetChaff( Node* chaff );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:
    NodeList* m_Children;
    int       m_bss_SuccessCounter;
};

class DynamicSelectorNodeGrist : public NodeGrist
{
public:
    DynamicSelectorNodeGrist();
    ~DynamicSelectorNodeGrist();

    int GetType() const { return E_GRIST_DYN_SELECTOR; }

    void GenerateConstructionCode( BehaviourTree* cb );
    void GenerateExecutionCode( BehaviourTree* cb );
    void GenerateDestructionCode( BehaviourTree* cb );
    void SetChaff( Node* n );

    void SetChildList( NodeList* children );
    NodeList* GetChildList();

private:
    typedef std::vector<int> PatchList; // <- TODO: KILL KILL KILL!

    NodeList*  m_Children;
    int        m_bss_NewBranch;
    int        m_bss_OldBranch;
    int        m_bss_JumpBackTarget;
    int        m_bss_RunningChild;
    PatchList m_DestructionCodeLocation;
};

class DecoratorNodeGrist : public VariableOwner
{
public:
    DecoratorNodeGrist();
    ~DecoratorNodeGrist();

    int GetType() const { return E_GRIST_DECORATOR; }

    void GenerateConstructionCode( BehaviourTree* bt );
    void GenerateExecutionCode( BehaviourTree* bt );
    void GenerateDestructionCode( BehaviourTree* bt );
    void SetChaff( Node* n );

    void SetDecorator( Decorator* dec );
    Decorator* GetDecorator();

    void SetChild( Node* child );
    Node* GetChild();

private:
    Decorator* m_Decorator;
    Node*      m_Child;
    int        m_bssPos;
    int        m_bssModPos;
};

class ActionNodeGrist : public VariableOwner
{
public:
	ActionNodeGrist();
	~ActionNodeGrist();

	int GetType() const { return E_GRIST_ACTION; }

	void GenerateConstructionCode( BehaviourTree* bt );
	void GenerateExecutionCode( BehaviourTree* bt );
	void GenerateDestructionCode( BehaviourTree* bt );

	void SetAction( Action* action );
	Action* GetAction();


private:

	Action*	m_Action;
	int		m_bssPos;
};

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
};

struct ActionGrist
{
	Action* m_Action;
};

/*
 * Prefixing "S" for now... don't want to break everything just yet.
 */
struct SNodeGrist
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
    BehaviourTree*  m_Tree;
    Node*           m_Parent;
    Identifier      m_Id;
    NodeGrist*      m_Grist;
    bool            m_IsChild;
    bool			m_Declared;
};

#endif /* BTREE_DATA_H_INCLUDED */
