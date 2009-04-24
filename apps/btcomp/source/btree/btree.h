/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BEHAVIOURTREE_H_
#define BEHAVIOURTREE_H_

#include "../code/bt_program.h"

class BehaviourTreeImpl;

class  Identifier;
class  Node;
class  Action;
class  Decorator;
class  NodeList;
class  NodeGrist;
struct Variable;
class  VariableList;
class  FunctionGrist;
class  CReturnState;
struct ParseFile;
struct ParserContext;
struct SPreProcessScope;
class  INodeVisitor;

class BehaviourTree
{
public:

    typedef const Node* const *       const_node_iterator;
    typedef const Action* const *    const_action_iterator;
    typedef const Decorator* const * const_decorator_iterator;

    BehaviourTree();
    ~BehaviourTree();

    const_node_iterator NodeBegin() const;
    const_node_iterator NodeEnd() const;

    const_action_iterator ActionBegin() const;
    const_action_iterator ActionEnd() const;

    const_decorator_iterator DecoratorBegin() const;
    const_decorator_iterator DecoratorEnd() const;

    void SetRootNode( Node* n );

    const char*  RegisterString( const char* str );

    Identifier* CreateId( const char*, int );
    void FreeId( Identifier* id );

    Node* LookupNode( Identifier* );
    Node* CreateNode( Identifier* );
    void FreeNode( Node* );

    Action* LookupAction( Identifier* );
    Action* CreateAction( Identifier* );
    void FreeAction( Action* );

    Decorator* LookupDecorator( Identifier* );
    Decorator* CreateDecorator( Identifier* );
    void FreeDecorator( Decorator* );

    NodeList* CreateNodeList();
    void FreeNodeList( NodeList* );

    NodeGrist* CreateNodeGrist( int grist_type );
    void FreeNodeGrist( NodeGrist* grist );

    FunctionGrist* CreateFunctionGrist();
    void FreeFunctionGrist( FunctionGrist* fg );

    Variable* CreateVariable();
    void FreeVariable( Variable* v );

    VariableList* CreateVariableList();
    void FreeVariableList( VariableList* vl );

    ParseFile* CreateParseFile();
    void FreeParseFile( ParseFile* pf );

    bool IsDefined( const char* );
    void Define( const char* );
    void Undefine( const char* );

    void Generate();
    void Print( FILE* outFile );
    bool Save( FILE* outFile, bool swapEndian ) const;

    int Parse( const char* filename );

    void Error( ParserContext*, int lineno, const char* msg );
    void Warning( ParserContext*, int lineno, const char* msg );

    void Visit( INodeVisitor* nv );

    Node*              m_Root;
    CodeSection        m_I;
    DataSection        m_D;
    BSSSection         m_B;
    BehaviourTreeImpl* m_Impl;
};



#endif /*BEHAVIOURTREE_H_*/
