/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef BTREE_HEADER_INCLUDED
#define BTREE_HEADER_INCLUDED

#include <stdio.h>

#include "btree_data.h"
#include "btree_func.h"

class BehaviorTreeImpl;

struct ParseFile;
struct ParserContext;
struct SPreProcessScope;

class BehaviorTree
{
public:

    typedef const Node* const *      const_node_iterator;
    typedef const Action* const *    const_action_iterator;
    typedef const Decorator* const * const_decorator_iterator;

    BehaviorTree();
    ~BehaviorTree();

    const_node_iterator NodeBegin() const;
    const_node_iterator NodeEnd() const;

    const_action_iterator ActionBegin() const;
    const_action_iterator ActionEnd() const;

    const_decorator_iterator DecoratorBegin() const;
    const_decorator_iterator DecoratorEnd() const;

    void SetRootNode( Node* n );

    const char* RegisterString( const char* str );
    const char* RegisterString( const char* str, hash_t hash );

    Node* LookupNode( const Identifier& );
    bool RegisterNode( Node* );
    void UnregisterNode( const Identifier& );

    Action* LookupAction( const Identifier& );
    bool RegisterAction( Action* );
    void UnregisterAction( const Identifier& );

    Decorator* LookupDecorator( const Identifier& );
    bool RegisterDecorator( Decorator* );
    void UnregisterDecorator( const Identifier& );

    ParseFile* CreateParseFile();
    void FreeParseFile( ParseFile* pf );

    bool IsDefined( const char* );
    void Define( const char* );
    void Undefine( const char* );

    void SetGenerateDebugInfo( bool debug_info_on );

    void Generate();
    void Print( FILE* outFile );
    bool Save( FILE* outFile, bool swapEndian ) const;

    int Parse( const char* filename );

    void Error( ParserContext*, int lineno, const char* msg );
    void Warning( ParserContext*, int lineno, const char* msg );

    Node*              m_Root;
    BehaviorTreeImpl* m_Impl;
};


#endif /*BTREE_HEADER_INCLUDED*/
