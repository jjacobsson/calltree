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

#include "endian.h"

#include <stdio.h>

using namespace callback;

BehaviourTree::BehaviourTree()
    : m_Root( 0x0 )
    , m_Impl( new BehaviourTreeImpl )
{
}

BehaviourTree::~BehaviourTree()
{
    delete m_Impl;
}

BehaviourTree::const_node_iterator BehaviourTree::NodeBegin() const
{
    return m_Impl->m_NodeTable.begin();
}

BehaviourTree::const_node_iterator BehaviourTree::NodeEnd() const
{
    return m_Impl->m_NodeTable.end();
}

BehaviourTree::const_action_iterator BehaviourTree::ActionBegin() const
{
    return m_Impl->m_ActionTable.begin();
}

BehaviourTree::const_action_iterator BehaviourTree::ActionEnd() const
{
    return m_Impl->m_ActionTable.end();
}

BehaviourTree::const_decorator_iterator BehaviourTree::DecoratorBegin() const
{
    return m_Impl->m_DecoratorTable.begin();
}

BehaviourTree::const_decorator_iterator BehaviourTree::DecoratorEnd() const
{
    return m_Impl->m_DecoratorTable.end();
}

CodeSection& BehaviourTree::GetCodeSection()
{
    return m_Impl->m_I;
}

DataSection& BehaviourTree::GetDataSection()
{
    return m_Impl->m_D;
}

BSSSection& BehaviourTree::GetBSSSection()
{
    return m_Impl->m_B;
}

void BehaviourTree::SetRootNode( Node* n )
{
    m_Root = n;
}

const char* BehaviourTree::RegisterString( const char* str )
{
    return m_Impl->m_StringTable.PutString( hashlittle( str ), str );
}

const char* BehaviourTree::RegisterString( const char* str, hash_t hash )
{
    return m_Impl->m_StringTable.PutString( hash, str );
}

Node* BehaviourTree::LookupNode( const Identifier& id )
{
    return m_Impl->m_NodeTable.Find( id );
}

bool BehaviourTree::RegisterNode( Node* n )
{
    if( m_Impl->m_NodeTable.Find( n->m_Id ) != 0x0 )
        return false;
    m_Impl->m_NodeTable.Insert( n );
    return true;
}

void BehaviourTree::UnregisterNode( const Identifier& id )
{
    m_Impl->m_NodeTable.Erase( id );
}

Action* BehaviourTree::LookupAction( const Identifier& id )
{
    return m_Impl->m_ActionTable.Find( id );
}

bool BehaviourTree::RegisterAction( Action* a )
{
    if( m_Impl->m_ActionTable.Find( a->m_Id ) != 0x0 )
        return false;
    m_Impl->m_ActionTable.Insert( a );
    return true;
}

void BehaviourTree::UnregisterAction( const Identifier& id )
{
	m_Impl->m_ActionTable.Erase( id );
}

Decorator* BehaviourTree::LookupDecorator( const Identifier& id )
{
    return m_Impl->m_DecoratorTable.Find( id );
}

bool BehaviourTree::RegisterDecorator( Decorator* d )
{
	if( m_Impl->m_DecoratorTable.Find( d->m_Id ) != 0x0 )
		return false;
	m_Impl->m_DecoratorTable.Insert( d );
	return true;
}

void BehaviourTree::UnregisterDecorator( const Identifier& id )
{
	m_Impl->m_DecoratorTable.Erase( id );
}

ParseFile* BehaviourTree::CreateParseFile()
{
    return m_Impl->m_ParseFilePool.Alloc();
}

void BehaviourTree::FreeParseFile( ParseFile* pf )
{
    m_Impl->m_ParseFilePool.Free( pf );
}

bool BehaviourTree::IsDefined( const char* str )
{
    hash_t hash = hashlittle( str );
    if( m_Impl->m_Defines.Exists( hash ) )
        return true;
    return false;
}

void BehaviourTree::Define( const char* str )
{
    hash_t hash = hashlittle( str );
    m_Impl->m_Defines.Insert( hash );
}

void BehaviourTree::Undefine( const char* str )
{
    hash_t hash = hashlittle( str );
    m_Impl->m_Defines.Erase( hash );
}

void BehaviourTree::SetGenerateDebugInfo( bool debug_info_on )
{
    m_Impl->m_I.SetGenerateDebugInfo( debug_info_on );
}

void BehaviourTree::Generate()
{
/*
    //Alloc storage area for bss header
    int bss_header  = m_Impl->m_B.Push( sizeof(BssHeader), 4 );
    //Alloc storage area for child-node return value.
    int bss_Return  = m_Impl->m_B.Push( sizeof(NodeReturns), 4 );

    //Jump past construction code if tree is already running
    m_Impl->m_I.Push( INST_JABC_C_EQUA_B, 0xffffffff, E_NODE_RUNNING, bss_Return );

    //Generate tree construction code
    m_Root->m_Grist->GenerateConstructionCode( this );

    //Patch jump past construction code instruction
    m_Impl->m_I.SetA1( 0, m_Impl->m_I.Count() );

    //Generate tree execution code
    m_Root->m_Grist->GenerateExecutionCode( this );

    //Store return value in bss.
    m_Impl->m_I.Push( INST__STORE_R_IN_B, bss_Return, 0, 0 );

    //Jump past destruciton code if tree is running
    int patch_jump_out = m_Impl->m_I.Count();
    m_Impl->m_I.Push( INST_JABC_R_EQUA_C, 0xffffffff, E_NODE_RUNNING, 0 );

    //Generate destruction code
    m_Root->m_Grist->GenerateDestructionCode( this );

    //Patch jump past destruction code instruction
    m_Impl->m_I.SetA1( patch_jump_out, m_Impl->m_I.Count() );

    //Suspend execution
    m_Impl->m_I.Push( INST_______SUSPEND, 0, 0, 0 );
*/
}

void BehaviourTree::Print( FILE* outFile )
{
    m_Impl->m_I.Print( outFile );
    m_Impl->m_B.Print( outFile );
    m_Impl->m_D.Print( outFile );
}

bool BehaviourTree::Save( FILE* outFile, bool swapEndian ) const
{
    ProgramHeader h;
    h.m_IC  = m_Impl->m_I.Count();
    h.m_DS  = 0;
    h.m_BS  = m_Impl->m_B.Size();

    if( swapEndian )
    {
        EndianSwap( h.m_IC );
        EndianSwap( h.m_DS );
        EndianSwap( h.m_BS );
    }
    size_t write  = sizeof( ProgramHeader );
    size_t written  = fwrite( &h, 1, write, outFile );
    if( written != write )
        return false;
    if( !m_Impl->m_I.Save( outFile, swapEndian ) )
        return false;
    if( !m_Impl->m_D.Save( outFile, swapEndian ) )
        return false;
    return true;
}

int BehaviourTree::Parse( const char* filename )
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


void BehaviourTree::Error( ParserContext* ctx, int lineno, const char* msg )
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

void BehaviourTree::Warning( ParserContext* ctx, int lineno, const char* msg )
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

