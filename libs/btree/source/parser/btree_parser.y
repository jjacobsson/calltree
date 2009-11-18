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
%error-verbose 
%verbose 

%start expressions

%{
#include "common.h"

#define YYMALLOC malloc
#define YYFREE free

bool DeclareAction( ParserContext* ctx, const Identifier& id, Variable* vars, Variable* args );
bool DeclareDecorator( ParserContext* ctx, const Identifier& id, Variable* vars, Variable* args );
bool DeclareNode( ParserContext* ctx, const Identifier& id, const NodeGrist& grist );

%}

%token            T_LPARE        /* '(' */
%token            T_RPARE        /* ')' */
%token            T_QUOTE        /* ''' */
%token            T_BTREE        /* literal string "btree" */
%token            T_INCLUDE      /* literal string "include" */
%token            T_SEQUENCE     /* literal string "sequence" */
%token            T_SELECTOR     /* literal string "selector" */
%token            T_DSELECTOR    /* literal string "dyn_selector" */
%token            T_PARALLEL     /* literal string "parallel" */
%token            T_ACTION       /* literal string "action" */
%token            T_DEFACT       /* literal string "defact" */
%token            T_DEFDEC       /* literal string "defdec" */
%token            T_DECORATOR    /* literal string "decorator" */
%token            T_INT32        /* literal string "int32" */
%token            T_BOOL         /* literal string "bool" */
%token            T_FLOAT        /* literal string "float" */
%token            T_STRING       /* literal string "string" */
%token            T_END_OF_FILE  /* end of file token */

%token<m_Integer> T_INT32_VALUE  /* a integer value */
%token<m_Bool>    T_BOOL_VALUE   /* a boolean value (i.e. "true" or "false) */
%token<m_Float>   T_FLOAT_VALUE  /* a float value */
%token<m_String>  T_STRING_VALUE /* a string value */
%token<m_Id>      T_ID           /* a legal identifier string */


%union {
    NodeGrist      m_NodeGrist;
    Identifier     m_Id;
    Node*          m_Node;
    Action*        m_Action;
    Decorator*     m_Decorator;
    Variable*      m_Variable;
    const char*    m_String;
    int            m_Integer;
    float          m_Float;
    bool           m_Bool;
}

%%

expressions: sexpr
           | sexpr expressions
           ;

sexpr: atom
     | T_QUOTE atom
     | list
     | T_QUOTE list
     ;
    
list: T_LPARE members T_RPARE
    | T_LPARE T_RPARE
    ;

members: btree
       | include
       | defact
       | defdec
       | sexpr
       | sexpr members
       ;

atom: T_ID                    {printf("id %s\n", $1.m_Text);}
    | T_INT32_VALUE           {printf("int32 %d\n", $1);}
    | T_STRING_VALUE          {printf("string %s\n", $1);}
    ;

btree: T_BTREE T_ID T_QUOTE node
     {
     	printf( "btree %s\n", $2.m_Text );
     }
     ;

include: T_INCLUDE T_STRING_VALUE
       {
       	printf( "include %s\n", $2 );
       }
       ;

defact: T_DEFACT T_ID vlist vdlist
	  {
	  	printf( "defact %s\n", $2.m_Text );
	  }
      ;
      
defdec: T_DEFDEC T_ID vlist vdlist
      {
      	printf( "defdec %s\n", $2.m_Text );
      }
      ;

node: T_LPARE sequence T_RPARE
    | T_LPARE selector T_RPARE
    | T_LPARE parallel T_RPARE
    | T_LPARE dselector T_RPARE
    | T_LPARE decorator T_RPARE
    | T_LPARE action T_RPARE
    ;
    
nlist: T_LPARE nmembers T_RPARE {printf("matched node list\n");}
     | T_LPARE T_RPARE          {printf("matched empty node list\n");}
     ;

nmembers: T_QUOTE node
        | T_QUOTE node nmembers
        ;

sequence: T_SEQUENCE nlist 
        {
        	printf("matched sequnce\n");
        }
        ;
        
selector: T_SELECTOR nlist 
        {
        	printf("matched selector\n");
        }
        ;
        
parallel: T_PARALLEL nlist 
        {
        	printf("matched parallel\n");
        }
        ;
        
dselector: T_DSELECTOR nlist
         {
         	printf("matched dynamic selector\n");
         }
         ;
         
decorator: T_DECORATOR T_QUOTE T_ID vlist T_QUOTE node 
         {
         	printf("matched decorator\n");
         }
         ;
         
action: T_ACTION T_QUOTE T_ID vlist 
      {
      	printf("matched action %s\n", $3.m_Text );
      }
      ;

vlist: T_LPARE vmember T_RPARE {printf("matched variable list\n");}
     | T_LPARE T_RPARE         {printf("matched empty variable list\n");}
     ;

vmember: variable
       | variable vmember
       ;
       
variable: T_QUOTE T_LPARE vtypes T_RPARE
        ;

vtypes: T_ID T_INT32_VALUE  {printf("matched int32 variable (%s %d)\n", $1.m_Text, $2 );}
      | T_ID T_STRING_VALUE {printf("matched string variable (%s %s)\n", $1.m_Text, $2 );}
      | T_ID T_BOOL_VALUE   {printf("matched bool variable (%s %f)\n", $1.m_Text, $2 );}
      | T_ID T_FLOAT_VALUE  {printf("matched float variable (%s %s)\n", $1.m_Text, $2?"true":"false" );}
      ;

vdlist: T_LPARE vdmember T_RPARE {printf("matched variable definition list\n");}
      | T_LPARE T_RPARE          {printf("matched empty variable definition list\n");}

vdmember: vardec
        | vardec vdmember
        ;

vardec: T_QUOTE T_LPARE vdtypes T_RPARE
      ;

vdtypes: T_INT32 T_ID
       | T_STRING T_ID
       | T_BOOL T_ID
       | T_FLOAT T_ID
       ;

%%

bool DeclareAction( ParserContext* ctx, const Identifier& id, Variable* vars, Variable* args )
{
    Action* a = ctx->m_Tree->LookupAction( id );
	if( !a )
	{
    	a = new Action;
    	InitAction( a );
    	a->m_Id = id;
		a->m_Args = args;
		a->m_Vars = vars;	
	    a->m_Declared = true;
	    ctx->m_Tree->RegisterAction( a );
    }
	else if( a && !a->m_Declared )
    {
    	a->m_Id = id;
    	a->m_Args = args;
    	a->m_Vars = vars;
    	a->m_Declared = true;
	}
    else if( a && a->m_Declared )
    {
    	DeleteVariableList( vars );
    	DeleteVariableList( args );
    	
        char tmp[2048];
        sprintf( tmp, "action \"%s\" was previously declared on line %d.\n", id.m_Text, a->m_Id.m_Line );
        yyerror( ctx, ctx->yyscanner, tmp );
        return false;
    }
    return true;
}

bool DeclareDecorator( ParserContext* ctx, const Identifier& id, Variable* vars, Variable* args )
{
    Decorator* d = ctx->m_Tree->LookupDecorator( id );
	if( !d )
	{
    	d = new Decorator;
    	InitDecorator( d );
    	d->m_Id = id;
		d->m_Args = args;
		d->m_Vars = vars;	
	    d->m_Declared = true;
	    ctx->m_Tree->RegisterDecorator( d );
    }
	else if( d && !d->m_Declared )
    {
    	d->m_Id = id;
    	d->m_Args = args;
    	d->m_Vars = vars;
    	d->m_Declared = true;
	}
    else if( d && d->m_Declared )
    {
    	DeleteVariableList( vars );
    	DeleteVariableList( args );
    	
        char tmp[2048];
        sprintf( tmp, "Decorator \"%s\" was previously declared on line %d.\n", id.m_Text, d->m_Id.m_Line );
        yyerror( ctx, ctx->yyscanner, tmp );
        return false;
    }
    return true;
}

bool DeclareNode( ParserContext* ctx, const Identifier& id, const NodeGrist& grist )
{
	Node* n = ctx->m_Tree->LookupNode( id );
	if( !n )
	{
		n = new Node;
		InitNode( n );
		n->m_Id = id;
		n->m_Grist = grist;
		n->m_Declared = true;
		ctx->m_Tree->RegisterNode( n );
		SetParentOnChildren( n );
	}
	else if( n && !n->m_Declared )
	{
		n->m_Id = id;
		n->m_Grist = grist;
		n->m_Declared = true;
		SetParentOnChildren( n );
	}
	else if( n && n->m_Declared )
	{
		char tmp[2048];
		sprintf( tmp, "Node \"%s\" was previously declared on line %d.\n", id.m_Text, n->m_Id.m_Line );
		yyerror( ctx, ctx->yyscanner, tmp );
		return false;
	}
	return true;
}

