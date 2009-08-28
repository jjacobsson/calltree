/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

%pure-parser
%parse-param { ParserContext* ctx }
%parse-param { void* scanner }
%lex-param   { yyscan_t* scanner }

%start nt_behaviour_tree

%{
#include "common.h"

#define YYMALLOC malloc
#define YYFREE free
%}

%token            T_ROOT         /* literal string "root" */
%token            T_NODE         /* literal string "node" */
%token<m_Id>      T_ID           /* a legal identifier string */
%token            T_COLON        /* ':' */
%token            T_SEMICOLON    /* ';' */
%token            T_COMMA        /* ',' */
%token            T_ASSIGNMENT   /* '=' */
%token            T_SEQUENCE     /* literal string "sequence" */
%token            T_SELECTOR     /* literal string "selector" */
%token            T_DSELECTOR    /* literal string "dyn_selector" */
%token            T_PARALLEL     /* literal string "parallel" */
%token            T_ACTION       /* literal string "action" */
%token            T_DECORATOR    /* literal string "decorator" */
%token            T_INT32        /* literal string "int32" */
%token            T_BOOL         /* literal string "bool" */
%token            T_FLOAT        /* literal string "float" */
%token            T_STRING       /* literal string "string" */
%token<m_Integer> T_INT32_VALUE  /* a integer value */
%token<m_Integer> T_BOOL_VALUE   /* a boolean value (i.e. "true" or "false) */
%token<m_Float>   T_FLOAT_VALUE  /* a float value */
%token<m_String>  T_STRING_VALUE /* a string value */
%token            T_END_OF_FILE  /* end of file token */

%union {
    Node*          m_Node;
    Identifier*    m_Id;
    NodeGrist*     m_NodeGrist;
    FunctionGrist* m_FuncGrist;
    NodeList*      m_NodeList;
    Action*        m_Action;
    Decorator*     m_Decorator;
    int            m_Integer;
    float          m_Float;
    const char*    m_String;
    Variable*      m_Variable;
	/*VariableList*  m_VariableList;*/
}

%type<m_Id>			  nt_id
%type<m_Node>	      nt_node_dec
%type<m_Node>		  nt_node_ref
%type<m_Action>		  nt_action_ref
%type<m_Decorator>	  nt_decorator_ref
%type<m_NodeGrist>	  nt_node_grist
%type<m_NodeGrist>	  nt_selector_node_grist
%type<m_NodeGrist>	  nt_dyn_selector_node_grist
%type<m_NodeGrist>	  nt_sequence_node_grist
%type<m_NodeGrist>	  nt_parallel_node_grist
%type<m_NodeGrist>	  nt_decorator_node_grist
%type<m_NodeGrist>	  nt_action_node_grist
%type<m_NodeList>	  nt_node_list
%type<m_Integer>	  nt_function_id
%type<m_Variable>	  nt_variable_dec
%type<m_Variable>	  nt_variable
%type<m_Variable>     nt_variable_dec_list
%type<m_Variable>     nt_variable_list
%type<m_FuncGrist>    nt_function_grist

%destructor { ctx->m_Tree->FreeId( $$ ); }            nt_id
%destructor { ctx->m_Tree->FreeId( $$ ); }		      T_ID
%destructor { ctx->m_Tree->FreeNode( $$ ); }	      nt_node_dec
%destructor { /* do nothing */ }					  nt_node_ref
%destructor { /* do nothing */ }					  nt_action_ref
%destructor { /* do nothing */ }					  nt_decorator_ref
%destructor { ctx->m_Tree->FreeNodeList( $$ ); }	  nt_node_list
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_node_grist
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_selector_node_grist
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_dyn_selector_node_grist
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_sequence_node_grist
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_parallel_node_grist
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_decorator_node_grist
%destructor { ctx->m_Tree->FreeNodeGrist( $$ ); }	  nt_action_node_grist
%destructor { DeleteVariableList( $$ ); }			  nt_variable_dec
%destructor { DeleteVariableList( $$ ); }			  nt_variable
%destructor { DeleteVariableList( $$ ); }			  nt_variable_dec_list
%destructor { DeleteVariableList( $$ ); }			  nt_variable_list
%destructor { ctx->m_Tree->FreeFunctionGrist( $$ ); } nt_function_grist

%%

nt_behaviour_tree
:
nt_declaration_list T_END_OF_FILE
{
    YYACCEPT;
}
;

nt_declaration_list
:
nt_declaration_list nt_declaration
|
nt_declaration
;

nt_declaration
:
nt_action_dec
|
nt_decorator_dec
|
nt_node_dec
|
nt_root_dec
|
T_SEMICOLON

;

nt_action_dec
:
T_ACTION T_COLON nt_id T_COLON nt_function_id nt_function_grist
{
    Action* a = ctx->m_Tree->CreateAction( $3 );
    if( !a )
    {
        a = ctx->m_Tree->LookupAction( $3 );

        char tmp[2048];
        sprintf( tmp, "action \"%s\" was previously declared on line %d.\n", $3->m_Text, a->m_Id->m_Line );
        yyerror( ctx, scanner, tmp );

        ctx->m_Tree->FreeFunctionGrist( $6 );
        ctx->m_Tree->FreeId( $3 );

        YYERROR;
    }
    a->m_FunctionId = $5;
    a->m_Grist      = $6;
}
;

nt_decorator_dec
:
T_DECORATOR T_COLON nt_id T_COLON nt_function_id T_COLON T_BOOL_VALUE T_COLON T_BOOL_VALUE nt_function_grist
{
    Decorator* d = ctx->m_Tree->CreateDecorator( $3 );
    if( !d )
    {
        d = ctx->m_Tree->LookupDecorator( $3 );


        char tmp[2048];
        sprintf( tmp, "decorator \"%s\" was previously declared on line %d.\n", $3->m_Text, d->m_Id->m_Line );
        yyerror( ctx, scanner, tmp );

        ctx->m_Tree->FreeFunctionGrist( $10 );
        ctx->m_Tree->FreeId( $3 );

        YYERROR;
    }
    d->m_Id         = $3;
    d->m_FunctionId = $5;
    d->m_Grist      = $10;
    d->m_Prune      = $7 == 1;
    d->m_Modify     = $9 == 1;
}
;

nt_function_id
:
T_INT32_VALUE
{
    $$ = $1;
}
;

nt_function_grist
:
T_COLON T_BOOL_VALUE T_COLON T_BOOL_VALUE T_COLON T_INT32_VALUE T_COLON nt_variable_dec_list T_SEMICOLON
{
    /* bss-value and variable list grist */
    $$ = ctx->m_Tree->CreateFunctionGrist();
    $$->m_bssSize   = $6;
    $$->m_Variables = $8;
    $$->m_Construct = $2 == 1;
    $$->m_Destruct  = $2 == 1;
}
|
T_COLON T_BOOL_VALUE T_COLON T_BOOL_VALUE T_COLON nt_variable_dec_list T_SEMICOLON
{
    /* only a variable declaration list grist */
    $$ = ctx->m_Tree->CreateFunctionGrist();
    $$->m_Variables = $6;
    $$->m_Construct = $2 == 1;
    $$->m_Destruct  = $4 == 1;
}
|
T_COLON T_BOOL_VALUE T_COLON T_BOOL_VALUE T_COLON T_INT32_VALUE T_SEMICOLON
{
    /* only a bss-value grist */
    $$ = ctx->m_Tree->CreateFunctionGrist();
    $$->m_bssSize   = $6;
    $$->m_Construct = $2 == 1;
    $$->m_Destruct  = $4 == 1;
}
|
T_COLON T_BOOL_VALUE T_COLON T_BOOL_VALUE T_SEMICOLON
{
    /* essentialy, no grist */
    $$ = ctx->m_Tree->CreateFunctionGrist();
    $$->m_Construct = $2 == 1;
    $$->m_Destruct  = $4 == 1;
}
;

nt_node_dec
:
T_NODE T_COLON nt_id T_COLON nt_node_grist
{
    Node* n = ctx->m_Tree->CreateNode( $3 );
    if( !n )
    {
        n = ctx->m_Tree->LookupNode( $3 );


        char tmp[2048];
        sprintf( tmp, "node \"%s\" was previously declared on line %d.\n", $3->m_Text, n->m_Id->m_Line );
        yyerror( ctx, scanner, tmp );

        ctx->m_Tree->FreeId( $3 );
        ctx->m_Tree->FreeNodeGrist( $5 );

        YYERROR;
    }

    n->m_Id     = $3;
    n->m_Grist  = $5;
    $5->SetChaff( n );
    $$ = n;
}
;

nt_root_dec
:
T_ROOT T_COLON nt_node_ref T_SEMICOLON
{
    ctx->m_Tree->SetRootNode( $3 );
}
;

nt_node_grist
:
nt_selector_node_grist
{
    $$ = $1;
}
|
nt_dyn_selector_node_grist
{
    $$ = $1;
}
|
nt_sequence_node_grist
{
    $$ = $1;
}
|
nt_parallel_node_grist
{
    $$ = $1;
}
|
nt_decorator_node_grist
{
    $$ = $1;
}
|
nt_action_node_grist
{
    $$ = $1;
}
;

nt_selector_node_grist
:
T_SELECTOR T_COLON nt_node_list T_SEMICOLON
{
    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_SELECTOR );
    static_cast<SelectorNodeGrist*>( $$ )->SetChildList( $3 );
}
;

nt_dyn_selector_node_grist
:
T_DSELECTOR T_COLON nt_node_list T_SEMICOLON
{
    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_DYN_SELECTOR );
    static_cast<DynamicSelectorNodeGrist*>( $$ )->SetChildList( $3 );
}
;

nt_sequence_node_grist
:
T_SEQUENCE T_COLON nt_node_list T_SEMICOLON
{
    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_SEQUENCE );
    static_cast<SequenceNodeGrist*>( $$ )->SetChildList( $3 );
}
;

nt_parallel_node_grist
:
T_PARALLEL T_COLON nt_node_list T_SEMICOLON
{
    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_PARALLEL );
    ParallelNodeGrist* grist = static_cast<ParallelNodeGrist*>( $$ );
    grist->SetChildList( $3 );
}
;

nt_decorator_node_grist
:
T_DECORATOR T_COLON nt_decorator_ref T_COLON nt_node_ref T_SEMICOLON
{
    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_DECORATOR );
    DecoratorNodeGrist* grist = static_cast<DecoratorNodeGrist*>($$);
    grist->SetDecorator( $3 );
    grist->SetChild( $5 );
}
|
T_DECORATOR T_COLON nt_decorator_ref T_COLON nt_node_ref T_COLON nt_variable_list T_SEMICOLON
{
    if( !$3->m_Grist->ValiadateVariables( $7 ) )
    {
        DeleteVariableList( $7 );
        yyerror( ctx, scanner, "variable list is invalid." );
        YYERROR;
    }

    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_DECORATOR );
    DecoratorNodeGrist* grist = static_cast<DecoratorNodeGrist*>($$);
    grist->SetDecorator( $3 );
    grist->SetChild( $5 );
    grist->SetVariableList( $7 );
}
;

nt_action_node_grist
:
T_ACTION T_COLON nt_action_ref T_SEMICOLON
{
    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_ACTION );
    static_cast<ActionNodeGrist*>($$)->SetAction( $3 );
}
|
T_ACTION T_COLON nt_action_ref T_COLON nt_variable_list T_SEMICOLON
{
    if( !$3->m_Grist->ValiadateVariables( $5 ) )
    {
        DeleteVariableList( $5 );
        yyerror( ctx, scanner, "variable list is invalid." );
        YYERROR;
    }

    $$ = ctx->m_Tree->CreateNodeGrist( E_GRIST_ACTION );
    ActionNodeGrist* grist = static_cast<ActionNodeGrist*>($$);
    grist->SetAction( $3 );
    grist->SetVariableList( $5 );
}
;

nt_node_list
:
/* empty */
{
    $$ = ctx->m_Tree->CreateNodeList();
}
|
nt_node_list T_COMMA nt_node_ref
{
    if( $3->m_IsChild  )
    {
        ctx->m_Tree->FreeNodeList( $1 );
        char tmp[2048];
        sprintf( tmp, "child node \"%s\" already has a parent.", $3->m_Id->m_Text );
        yyerror( ctx, scanner, tmp );
        YYERROR;
    }
    if( !$1->Append( $3 ) )
    {
        ctx->m_Tree->FreeNodeList( $1 );
        yyerror( ctx, scanner, "number of allowed child-nodes exceeded." );
        YYERROR;
    }
    $$ = $1;
}
|
nt_node_ref
{
    if( $1->m_IsChild  )
    {
        char tmp[2048];
        sprintf( tmp, "child node \"%s\" has a parent.", $1->m_Id->m_Text );
        yyerror( ctx, scanner, tmp );
        YYERROR;
    }
    NodeList* cl = ctx->m_Tree->CreateNodeList();
    if( !cl->Append( $1 ) )
    {
        ctx->m_Tree->FreeNodeList( cl );
        yyerror( ctx, scanner, "number of allowed child-nodes exceeded." );
        YYERROR;
    }
    $$ = cl;
}
;


nt_node_ref
:
nt_id
{
    Node* n = ctx->m_Tree->LookupNode( $1 );
    if( n == 0x0 )
    {
        char tmp[2048];
        sprintf( tmp, "node \"%s\" has not been declared.\n", $1->m_Text );
        ctx->m_Tree->FreeId( $1 );
        yyerror( ctx, scanner, tmp );
        YYERROR;
    }
    $$ = n;
}
;

nt_action_ref
:
nt_id
{
    Action* a = ctx->m_Tree->LookupAction( $1 );
    if( a == 0x0 )
    {
        char tmp[2048];
        sprintf( tmp, "action \"%s\" has not been declared.\n", $1->m_Text );
        ctx->m_Tree->FreeId( $1 );
        yyerror( ctx, scanner, tmp );
        YYERROR;
    }
    $$ = a;
}
;

nt_decorator_ref
:
nt_id
{
    Decorator* d = ctx->m_Tree->LookupDecorator( $1 );
    if( d == 0x0 )
    {
        char tmp[2048];
        sprintf( tmp, "decorator \"%s\" has not been declared.", $1->m_Text );
        ctx->m_Tree->FreeId( $1 );
        yyerror( ctx, scanner, tmp );
        YYERROR;
    }
    $$ = d;
}
;

nt_id
:
T_ID
{
    $$ = $1;
}
;

nt_variable_dec_list
:
/* empty */
{
    $$ = 0x0;
}
|
nt_variable_dec_list T_COMMA nt_variable_dec
{
	if( $1 )
	{
		Variable* v = FindLastVariable( $1 );
		v->m_Next = $3;
		$$ = $1;
	}	
	else
	{
		$$ = $3;
	}
}
|
nt_variable_dec
{
	$$ = $1;
}
;

nt_variable_dec
:
T_STRING T_ID
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_STRING;
    $$->m_Id            = $2;
    $$->m_Data.m_Int    = 0;
    $$->m_Next			= 0x0;    
}
|
T_INT32 T_ID
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_INTEGER;
    $$->m_Id            = $2;
    $$->m_Data.m_Int    = 0;
    $$->m_Next			= 0x0;    
}
|
T_FLOAT T_ID
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_FLOAT;
    $$->m_Id            = $2;
    $$->m_Data.m_Int    = 0;
    $$->m_Next			= 0x0;    
}
|
T_BOOL T_ID
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_BOOL;
    $$->m_Id            = $2;
    $$->m_Data.m_Int    = 0;
    $$->m_Next			= 0x0;
}
;

nt_variable_list
:
/* empty */
{
    $$ = 0x0;
}
|
nt_variable_list T_COMMA nt_variable
{
	if( $1 )
	{
		Variable* v = FindLastVariable( $1 );
		v->m_Next = $3;
		$$ = $1;
	}	
	else
	{
		$$ = $3;
	}
}
|
nt_variable
{
	$$ = $1;
}
;

nt_variable
:
T_ID T_ASSIGNMENT T_STRING_VALUE
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_STRING;
    $$->m_Id            = $1;
    $$->m_Data.m_String = $3;
    $$->m_Next			= 0x0;
}
|
T_ID T_ASSIGNMENT T_INT32_VALUE
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_INTEGER;
    $$->m_Id            = $1;
    $$->m_Data.m_Int    = $3;
    $$->m_Next			= 0x0;    
}
|
T_ID T_ASSIGNMENT T_FLOAT_VALUE
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_FLOAT;
    $$->m_Id            = $1;
    $$->m_Data.m_Float  = $3;
    $$->m_Next			= 0x0;    
}
|
T_ID T_ASSIGNMENT T_BOOL_VALUE
{
    $$                  = new Variable;
    $$->m_Type          = Variable::E_VART_BOOL;
    $$->m_Id            = $1;
    $$->m_Data.m_Int    = $3;
    $$->m_Next			= 0x0;    
}
;




