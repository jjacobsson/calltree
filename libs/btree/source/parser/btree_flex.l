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

%option outfile="btree_flex.c"
%option reentrant 
%option bison-bridge
%option nounistd
%option never-interactive
%option 8bit
%option noyywrap

%option noyyalloc noyyrealloc noyyfree

%{

#include "parser.h"
#include <other/lookup3.h>
#include <stdio.h>
#include <string.h> // <- for memcpy.... yuck.
#define YY_INPUT(inBuffer, outResult, inMaxSize) ((outResult) = yyget_extra(yyscanner)->m_Funcs.m_Read( yyget_extra(yyscanner), (inBuffer), (inMaxSize)) )
#define STRINGBUFFERAPPEND( STR1, STR2 ) \
	{ \
		ParserContext ctx = yyget_extra(yyscanner); \
		append( &(ctx->m_Parsed), (STR1) ); \
		append( &(ctx->m_Original), (STR2) ); \
	} \
		
%}

CHAR            ([a-zA-Z_])
ID              {CHAR}([a-zA-Z0-9_])*
WHITESPACE      ([\t\r ])
NEWLINE         ([\n])
INTEGER         -{0,1}([[:digit:]])+
HEX_INTEGER     0[xX][[:xdigit:]]+
PPINCSTART      (\"|\<)
PPINCSTOP       (\"|\>)

dseq            ([[:digit:]]+)
dseq_opt        ([[:digit:]]*)
frac            (({dseq_opt}"."{dseq})|{dseq}".")
exp             ([eE][+-]?{dseq})
exp_opt         ({exp}?)
fsuff           [flFL]
fsuff_opt       ({fsuff}?)
hpref           (0[xX])
hdseq           ([[:xdigit:]]+)
hdseq_opt       ([[:xdigit:]]*)
hfrac           (({hdseq_opt}"."{hdseq})|({hdseq}"."))
bexp            ([pP][+-]?{dseq})
dfc             (({frac}{exp_opt}{fsuff_opt})|({dseq}{exp}{fsuff_opt}))
hfc             (({hpref}{hfrac}{bexp}{fsuff_opt})|({hpref}{hdseq}{bexp}{fsuff_opt}))

FLOAT           ({dfc}|{hfc})

%x string


%%

"("             { return T_LPARE; }
")"             { return T_RPARE; }
"'"             { return T_QUOTE; }
deftree         { return T_DEFTREE; }
defact          { return T_DEFACT; }
defdec          { return T_DEFDEC; }
tree            { return T_TREE; }
sequence        { return T_SEQUENCE; }
selector        { return T_SELECTOR; }
parallel        { return T_PARALLEL; }
dyn_selector    { return T_DSELECTOR; }
succeed         { return T_SUCCEED; }
fail            { return T_FAIL; }
work            { return T_WORK; }
action          { return T_ACTION; }
decorator       { return T_DECORATOR; }
int32           { return T_INT32; }
bool            { return T_BOOL; }
float           { return T_FLOAT; }
string          { return T_STRING; }
include         { return T_INCLUDE; }
true            { yylval->m_Bool = true; return T_BOOL_VALUE; }
false           { yylval->m_Bool = false; return T_BOOL_VALUE; }
{ID}            {
                    SParserContext* ctx = yyget_extra(yyscanner);
                    (*yylval).m_Id.m_Hash = hashlittle( yytext );
                    (*yylval).m_Id.m_Line = ctx->m_LineNo;
                    (*yylval).m_Id.m_Text = register_string( ctx->m_Tree, yytext, (*yylval).m_Id.m_Hash );
                    return T_ID; 
                }
{INTEGER}       {
                    sscanf( yytext, "%d", &((*yylval).m_Integer) ); 
                    return T_INT32_VALUE;
                }
{HEX_INTEGER}   {
                    sscanf( yytext, "%x", &((*yylval).m_Integer) ); 
                    return T_INT32_VALUE;
                }
{FLOAT}         {
                    sscanf( yytext, "%f", &((*yylval).m_Float) );
                    return T_FLOAT_VALUE;
                }
{NEWLINE}       {
                    yyget_extra(yyscanner)->m_LineNo++;
                    /* eat newlines, or they get echoed to the screen... */ 
                }
{WHITESPACE}    {
                    /* eat whitespace */ 
                }

;.*				{ /* eat comments */ }

\"              { BEGIN(string); }

.               { yyerror( yyget_extra(yyscanner), "unrecognized character." ); }

<string>{

\"              {   /*closing quote*/
                    BEGIN(INITIAL);
                    SParserContext* ctx = yyget_extra( yyscanner );
                    (*yylval).m_String.m_Parsed = register_string( ctx->m_Tree, ctx->m_Parsed.m_Str );
                    (*yylval).m_String.m_Raw = register_string( ctx->m_Tree, ctx->m_Original.m_Str );
                    clear( &ctx->m_Parsed );
                    clear( &ctx->m_Original );
                    return T_STRING_VALUE;
                }
\\n             { STRINGBUFFERAPPEND( '\n', yytext ); }
\\t             { STRINGBUFFERAPPEND( '\t', yytext ); }
\\r             { STRINGBUFFERAPPEND( '\r', yytext ); }
\\b             { STRINGBUFFERAPPEND( '\b', yytext ); }
\\f             { STRINGBUFFERAPPEND( '\f', yytext ); }
\\(.|\n)        { 
					ParserContext ctx = yyget_extra(yyscanner);
					append( &(ctx->m_Parsed), yytext[1] );
					append( &(ctx->m_Original), yytext );
				}
[^\\\n\"]+      { STRINGBUFFERAPPEND( yytext, yytext ); }

([\n])			{ yyerror( yyget_extra(yyscanner), "new-line in string." ); }

}

%%

/* Provide our own implementations. */
void * yyalloc (size_t bytes, void* yyscanner)
{
	return yyget_extra(yyscanner)->m_Allocator.m_Alloc( bytes );
}
 
void * yyrealloc (void * ptr, size_t bytes, void* yyscanner)
{
	void* tmp = yyalloc( bytes, yyscanner );
	memcpy( tmp, ptr, bytes );
	yyfree( ptr, yyscanner );
	return tmp;
}
 
void yyfree (void * ptr, void * yyscanner) 
{
	yyget_extra(yyscanner)->m_Allocator.m_Free( ptr );
}
