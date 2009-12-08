/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <btree/btree.h>

#include <other/lookup3.h>

#include <callback/callback.h>
#include <callback/instructions.h>

#include "btree_impl.h"
#include <btree/btree_data.h>
#include <btree/btree_func.h>

#include "../parser/common.h"

//#include "endian.h"

#include <stdio.h>

using namespace callback;

BehaviorTree::BehaviorTree()
    : m_Root( 0x0 )
    , m_Impl( new BehaviorTreeImpl )
{
}

BehaviorTree::~BehaviorTree()
{
    delete m_Impl;
}

void BehaviorTree::SetRootNode( Node* n )
{
    m_Root = n;
}

const char* BehaviorTree::RegisterString( const char* str )
{
    return m_Impl->m_StringTable.PutString( hashlittle( str ), str );
}

const char* BehaviorTree::RegisterString( const char* str, hash_t hash )
{
    return m_Impl->m_StringTable.PutString( hash, str );
}

Action* BehaviorTree::LookupAction( const Identifier& id )
{
    return m_Impl->m_ActionTable.Find( id );
}

bool BehaviorTree::RegisterAction( Action* a )
{
    if( m_Impl->m_ActionTable.Find( a->m_Id ) != 0x0 )
        return false;
    m_Impl->m_ActionTable.Insert( a );
    return true;
}

void BehaviorTree::UnregisterAction( const Identifier& id )
{
	m_Impl->m_ActionTable.Erase( id );
}

Decorator* BehaviorTree::LookupDecorator( const Identifier& id )
{
    return m_Impl->m_DecoratorTable.Find( id );
}

bool BehaviorTree::RegisterDecorator( Decorator* d )
{
	if( m_Impl->m_DecoratorTable.Find( d->m_Id ) != 0x0 )
		return false;
	m_Impl->m_DecoratorTable.Insert( d );
	return true;
}

void BehaviorTree::UnregisterDecorator( const Identifier& id )
{
	m_Impl->m_DecoratorTable.Erase( id );
}

ParseFile* BehaviorTree::CreateParseFile()
{
    return m_Impl->m_ParseFilePool.Alloc();
}

void BehaviorTree::FreeParseFile( ParseFile* pf )
{
    m_Impl->m_ParseFilePool.Free( pf );
}

int BehaviorTree::Parse( const char* filename )
{

    ParserContext ctx;

    yylex_init(&ctx.yyscanner);
    yyset_extra(&ctx, ctx.yyscanner);

    ctx.m_Tree      = this;

    if( !push_parse_file( &ctx, filename ) )
        return -1;

    int r = yyparse(&ctx, ctx.yyscanner);

    yylex_destroy(ctx.yyscanner);


    return r;
}


void BehaviorTree::Error( ParserContext* ctx, int lineno, const char* msg )
{
    if( ctx->m_File  )
    {
        fprintf( stdout, "%s(%d) : error : %s\n", ctx->m_File->m_Name, lineno, msg );
    }
    else
    {
        fprintf( stdout, "<no file>(%d) : error : %s\n", lineno, msg );
    }
}

void BehaviorTree::Warning( ParserContext* ctx, int lineno, const char* msg )
{
    if( ctx->m_File  )
    {
        fprintf( stdout, "%s(%d) : warning : %s\n", ctx->m_File->m_Name, lineno, msg );
    }
    else
    {
        fprintf( stdout, "<no file>(%d) : warning : %s\n", lineno, msg );
    }
}

