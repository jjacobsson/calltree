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

    BehaviorTree();
    ~BehaviorTree();

    void SetRootNode( Node* n );

    const char* RegisterString( const char* str );
    const char* RegisterString( const char* str, hash_t hash );

    Action* LookupAction( const Identifier& );
    bool RegisterAction( Action* );
    void UnregisterAction( const Identifier& );

    Decorator* LookupDecorator( const Identifier& );
    bool RegisterDecorator( Decorator* );
    void UnregisterDecorator( const Identifier& );

    ParseFile* CreateParseFile();
    void FreeParseFile( ParseFile* pf );

    void SetGenerateDebugInfo( bool debug_info_on );

    int Parse( const char* filename );

    void Error( ParserContext*, int lineno, const char* msg );
    void Warning( ParserContext*, int lineno, const char* msg );

    Node*              m_Root;
    BehaviorTreeImpl* m_Impl;
};


#endif /*BTREE_HEADER_INCLUDED*/
