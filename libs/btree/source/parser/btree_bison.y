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

%pure-parser
%parse-param { SParserContext* ctx }
%parse-param { void* scanner }
%lex-param   { yyscan_t* scanner }
%error-verbose 
%verbose 

%start expressions

%{

#include "../btree_internal.h"
#include "parser.h"
#include <stdio.h>

#define YYMALLOC ctx->m_Allocator.m_Alloc
#define YYFREE ctx->m_Allocator.m_Free

bool declare_action( SParserContext* ctx, const Identifier& id, Parameter* vars, Parameter* args );
bool declare_decorator( SParserContext* ctx, const Identifier& id, Parameter* vars, Parameter* args );
Node* allocate_node( BehaviorTreeContext ctx, NodeGristType type, Node* child, const char* buffer, unsigned int line_no );
Parameter* allocate_parameter( BehaviorTreeContext ctx, ParameterType type, const Identifier& id, const char* buffer, unsigned int line_no );

#define ALLOCATE_NODE( TYPE, CHILD ) allocate_node( ctx->m_Tree, TYPE, CHILD, ctx->m_Current, ctx->m_LineNo );
#define ALLOCATE_PARAMETER( TYPE, ID ) allocate_parameter( ctx->m_Tree, TYPE, ID, ctx->m_Current, ctx->m_LineNo );

%}

%token            T_LPARE        /* '(' */
%token            T_RPARE        /* ')' */
%token            T_QUOTE        /* ''' */
%token            T_DEFTREE      /* literal string "deftree" */
%token            T_DEFACT       /* literal string "defact" */
%token            T_DEFDEC       /* literal string "defdec" */
%token            T_TREE         /* literal string "tree" */
%token            T_INCLUDE      /* literal string "include" */
%token            T_SEQUENCE     /* literal string "sequence" */
%token            T_SELECTOR     /* literal string "selector" */
%token            T_DSELECTOR    /* literal string "dyn_selector" */
%token            T_PARALLEL     /* literal string "parallel" */
%token            T_SUCCEED      /* literal string "succeed" */
%token            T_FAIL         /* literal string "fail" */
%token            T_WORK         /* literal string "work" */
%token            T_ACTION       /* literal string "action" */
%token            T_DECORATOR    /* literal string "decorator" */
%token            T_INT32        /* literal string "int32" */
%token            T_BOOL         /* literal string "bool" */
%token            T_FLOAT        /* literal string "float" */
%token            T_STRING       /* literal string "string" */
%token            T_HASH         /* literal string "hash" */

%token<m_Integer> T_INT32_VALUE  /* a integer value */
%token<m_Bool>    T_BOOL_VALUE   /* a boolean value (i.e. "true" or "false) */
%token<m_Float>   T_FLOAT_VALUE  /* a float value */
%token<m_String>  T_STRING_VALUE /* a string value */
%token<m_Id>      T_ID           /* a legal identifier string */

%type<m_Node> node nmembers sequence selector parallel dselector succeed fail work decorator action tree nlist
%type<m_Parameter> vlist vmember Parameter vtypes vdlist vdmember vardec vdtypes

%union {
    NodeGrist      m_NodeGrist;
    Identifier     m_Id;
    Node*          m_Node;
    Action*        m_Action;
    Decorator*     m_Decorator;
    Parameter*      m_Parameter;
    StringData     m_String;
    int            m_Integer;
    float          m_Float;
    bool           m_Bool;
}

%%

expressions: list
           | list expressions
           ;

list: T_LPARE atom T_RPARE
    | T_LPARE T_RPARE
    ;

atom: deftree
    | include
    | defact
    | defdec
    ;

deftree: T_DEFTREE T_ID nlist
       {
		BehaviorTree* t = look_up_behavior_tree( ctx->m_Tree, &$2 );
		t->m_Root = $3;
		t->m_Declared = true;
		t->m_Locator.m_Buffer = ctx->m_Current;
		t->m_Locator.m_LineNo = ctx->m_LineNo;
		set_parent_on_children( t );
       }
       ;

include: T_INCLUDE T_STRING_VALUE
       {
       	Include inc;
       	inc.m_Name		= ctx->m_Funcs.m_Translate( ctx, $2.m_Raw );
       	inc.m_Parent	= ctx->m_Current;
       	inc.m_LineNo	= ctx->m_LineNo;
       	add_include( ctx->m_Tree, inc );
       }
       ;

defact: T_DEFACT T_ID vlist vdlist
	  {
		Action* a = look_up_action( ctx->m_Tree, &$2 );
		a->m_Options = $3;
		a->m_Declarations = $4;
		a->m_Declared = true;
		a->m_Locator.m_Buffer = ctx->m_Current;
		a->m_Locator.m_LineNo = ctx->m_LineNo;
	  }
      ;
      
defdec: T_DEFDEC T_ID vlist vdlist
      {
      	Decorator* d = look_up_decorator( ctx->m_Tree, &$2 );
      	d->m_Options = $3;
      	d->m_Declarations = $4;
      	d->m_Declared = true;
      	d->m_Locator.m_Buffer = ctx->m_Current;
      	d->m_Locator.m_LineNo = ctx->m_LineNo;
      }
      ;

node: T_LPARE sequence T_RPARE  { $$ = $2; }
    | T_LPARE selector T_RPARE  { $$ = $2; }
    | T_LPARE parallel T_RPARE  { $$ = $2; }
    | T_LPARE dselector T_RPARE { $$ = $2; }
    | T_LPARE succeed T_RPARE   { $$ = $2; }
    | T_LPARE fail T_RPARE      { $$ = $2; }
    | T_LPARE work T_RPARE      { $$ = $2; }
    | T_LPARE decorator T_RPARE { $$ = $2; }
    | T_LPARE action T_RPARE    { $$ = $2; }
    | T_LPARE tree T_RPARE      { $$ = $2; }
    ;
    
nlist: T_LPARE nmembers T_RPARE         { $$ = $2; }
     | T_QUOTE T_LPARE nmembers T_RPARE { $$ = $3; }
     | T_QUOTE T_LPARE T_RPARE          { $$ = 0x0;}
     ;

nmembers: node	{ $$ = $1; }
        | node nmembers { $$ = $1; append_to_end( $1, $2 ); }
        ;

sequence: T_SEQUENCE nlist 
        {
        	Node* n = ALLOCATE_NODE( E_GRIST_SEQUENCE, $2 );
        	$$ = n;
        }
        ;
        
selector: T_SELECTOR nlist 
        {
        	Node* n = ALLOCATE_NODE( E_GRIST_SELECTOR, $2 );
        	$$ = n;
        }
        ;
        
parallel: T_PARALLEL nlist 
        {
        	Node* n = ALLOCATE_NODE( E_GRIST_PARALLEL, $2 );
        	$$ = n;
        }
        ;
        
dselector: T_DSELECTOR nlist
         {
        	Node* n = ALLOCATE_NODE( E_GRIST_DYN_SELECTOR, $2 );
        	$$ = n;
         }
         ;
         
succeed: T_SUCCEED
         {
        	Node* n = ALLOCATE_NODE( E_GRIST_SUCCEED, 0x0 );
        	$$ = n;
         }
         ;
         
fail: T_FAIL
         {
        	Node* n = ALLOCATE_NODE( E_GRIST_FAIL, 0x0 );
        	$$ = n;
         }
         ;
         
work: T_WORK
         {
        	Node* n = ALLOCATE_NODE( E_GRIST_WORK, 0x0 );
        	$$ = n;
         }
         ;
         
decorator: T_DECORATOR T_QUOTE T_ID vlist node 
         {
        	Node* n = ALLOCATE_NODE( E_GRIST_DECORATOR, $5 );
        	$$ = n;
        	n->m_Grist.m_Decorator.m_Parameters = $4;
        	n->m_Grist.m_Decorator.m_Decorator = look_up_decorator( ctx->m_Tree, &$3 );
         }
         | T_DECORATOR T_QUOTE T_ID vlist T_QUOTE T_LPARE T_RPARE
         {
        	Node* n = ALLOCATE_NODE( E_GRIST_DECORATOR, 0x0 );
        	$$ = n;
        	n->m_Grist.m_Decorator.m_Parameters = $4;
        	n->m_Grist.m_Decorator.m_Decorator = look_up_decorator( ctx->m_Tree, &$3 );
         }
         ;
         
         
action: T_ACTION T_QUOTE T_ID vlist 
      {
        	Node* n = ALLOCATE_NODE( E_GRIST_ACTION, 0x0 );
        	$$ = n;
        	n->m_Grist.m_Action.m_Parameters = $4;
        	n->m_Grist.m_Action.m_Action = look_up_action( ctx->m_Tree, &$3 );
      }
      ;

tree: T_TREE T_QUOTE T_ID 
    {
    	Node* n = ALLOCATE_NODE( E_GRIST_TREE, 0x0 );
    	$$ = n;
    	n->m_Grist.m_Tree.m_Tree = look_up_behavior_tree( ctx->m_Tree, &$3 );
    }

vlist: T_QUOTE T_LPARE vmember T_RPARE { $$ = $3; }
     | T_QUOTE T_LPARE T_RPARE         { $$ = 0x0; }
     ;

vmember: Parameter         { $$ = $1; }
       | Parameter vmember { $$ = $1; append_to_end( $$, $2 ); }
       ;
       
Parameter: T_LPARE vtypes T_RPARE { $$ = $2; }
        ;

vtypes: T_ID T_INT32_VALUE  { $$ = ALLOCATE_PARAMETER( E_VART_INTEGER, $1 ); $$->m_Data.m_Integer = $2; $$->m_ValueSet = true; }
      | T_ID T_STRING_VALUE { $$ = ALLOCATE_PARAMETER( E_VART_STRING, $1 );  $$->m_Data.m_String = $2; $$->m_ValueSet = true; }
      | T_ID T_BOOL_VALUE   { $$ = ALLOCATE_PARAMETER( E_VART_BOOL, $1 );    $$->m_Data.m_Bool = $2; $$->m_ValueSet = true; }
      | T_ID T_FLOAT_VALUE  { $$ = ALLOCATE_PARAMETER( E_VART_FLOAT, $1 );   $$->m_Data.m_Float = $2; $$->m_ValueSet = true; }
      ;

vdlist: T_QUOTE T_LPARE vdmember T_RPARE { $$ = $3; }
      | T_QUOTE T_LPARE T_RPARE          { $$ = 0x0; }

vdmember: vardec          { $$ = $1; }
        | vardec vdmember { $$ = $1; append_to_end( $$, $2 ); }
        ;

vardec: T_LPARE vdtypes T_RPARE { $$ = $2; }
      ;

vdtypes: T_INT32 T_ID  { $$ = ALLOCATE_PARAMETER( E_VART_INTEGER, $2 ); }
       | T_STRING T_ID { $$ = ALLOCATE_PARAMETER( E_VART_STRING, $2 ); }
       | T_BOOL T_ID   { $$ = ALLOCATE_PARAMETER( E_VART_BOOL, $2 ); }
       | T_FLOAT T_ID  { $$ = ALLOCATE_PARAMETER( E_VART_FLOAT, $2 ); }
       | T_HASH T_ID   { $$ = ALLOCATE_PARAMETER( E_VART_HASH, $2 ); }
       ;

%%

Node* allocate_node( BehaviorTreeContext ctx, NodeGristType type, Node* child, const char* buffer, unsigned int line_no )
{
	Node* n = (Node*)allocate_object( ctx );
	if( n )
	{
		init( n );
		n->m_Grist.m_Type = type;
		n->m_NodeId = ctx->m_NodeId++;
		n->m_Locator.m_Buffer = buffer;
		n->m_Locator.m_LineNo = line_no;		
		set_first_child( n, child );
		set_parent_on_children( n );
	}
	return n;
}

Parameter* allocate_parameter( BehaviorTreeContext ctx, ParameterType type, const Identifier& id, const char* buffer, unsigned int line_no )
{
	Parameter* p = (Parameter*)allocate_object( ctx );
	if( p )
	{
		init( p );
		p->m_Locator.m_Buffer = buffer;
		p->m_Locator.m_LineNo = line_no;
		p->m_Type = type;
		p->m_Id = id;
	}
	return p;
} 


