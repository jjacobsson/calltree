/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

%option outfile="btree_lexer.c"
%option reentrant 
%option bison-bridge
%option nounistd
%option never-interactive
%option 8bit
%option noyywrap
%option yylineno

%{

#include "common.h"
#include <other/lookup3.h>
#include <stdio.h>
#define YY_INPUT(inBuffer, outResult, inMaxSize) ((outResult) = fill_lex_buffer( yyget_extra(yyscanner), (inBuffer), (inMaxSize)))

#define EOF_DANCE                                                           \
    {                                                                       \
        ParserContext* ctx = yyget_extra(yyscanner);                        \
        if( ctx->m_PPCurrScope != 0 ) {                                     \
            yyerror( ctx, yyscanner, "#if/#endif miss-match." );            \
            yyterminate();                                                  \
        }                                                                   \
        else if( pop_parse_file( ctx ) )                                    \
            return T_END_OF_FILE;                                           \
    }                                                                       \
        
        

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

%x preprocessor pp_eat_statement pp_include pp_inc_string pp_ifdef pp_ifndef pp_eat_until pp_define pp_undef
%x ml_comment
%x string


%%

<INITIAL><<EOF>>    { EOF_DANCE; }


":"             { return T_COLON; }
";"             { return T_SEMICOLON; }
","             { return T_COMMA; }
"="             { return T_ASSIGNMENT; }
root            { return T_ROOT; }
node            { return T_NODE; }
sequence        { return T_SEQUENCE; }
selector        { return T_SELECTOR; }
dyn_selector    { return T_DSELECTOR; }
parallel        { return T_PARALLEL; }
action          { return T_ACTION; }
decorator       { return T_DECORATOR; }
int32           { return T_INT32; }
bool            { return T_BOOL; }
float           { return T_FLOAT; }
string          { return T_STRING; }
true            { yylval->m_Bool = true; return T_BOOL_VALUE; }
false           { yylval->m_Bool = false; return T_BOOL_VALUE; }
{ID}            {
                    ParserContext* ctx = yyget_extra(yyscanner); 
                    (*yylval).m_Id.m_Hash = hashlittle( yytext );
                    (*yylval).m_Id.m_Line = yylineno;
                    (*yylval).m_Id.m_Text = ctx->m_Tree->RegisterString( yytext, (*yylval).m_Id.m_Hash );
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
{NEWLINE}       { /* eat newlines, or they get echoed to the screen... */ }             
{WHITESPACE}    { /* eat whitespace */ }


"//".*          { /* eat single line comments */ }

\"              { BEGIN(string); }
"/*"            { BEGIN(ml_comment); }
#               { BEGIN(preprocessor); }
.               { yyerror( yyget_extra(yyscanner), yyscanner, "unrecognized character." ); }

<string>{
\"              {   /*closing quote*/
                    BEGIN(INITIAL);
                    ParserContext* ctx = yyget_extra( yyscanner );
                    (*yylval).m_String  = ctx->m_Tree->RegisterString( ctx->m_TmpString.c_str() );
                    ctx->m_TmpString    = "";
                    return T_STRING_VALUE;
                }
\\n             { yyget_extra(yyscanner)->m_TmpString += '\n'; }
\\t             { yyget_extra(yyscanner)->m_TmpString += '\t'; }
\\r             { yyget_extra(yyscanner)->m_TmpString += '\r'; }
\\b             { yyget_extra(yyscanner)->m_TmpString += '\b'; }
\\f             { yyget_extra(yyscanner)->m_TmpString += '\f'; }

\\(.|\n)        { yyget_extra(yyscanner)->m_TmpString += yytext[1]; }

[^\\\n\"]+      {
                    yyget_extra(yyscanner)->m_TmpString += yytext;
                }
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file in string constant" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}

<ml_comment>{
[^*\n]*         { }
[^*\n]*\n       { }
"*"+[^*/\n]*    { }
"*"+[^*/\n]*\n  { }
"*"+"/"         { BEGIN(INITIAL); }
}

<preprocessor>{
{WHITESPACE}    { /* eat whitespace */ }
ifdef           { BEGIN(pp_ifdef); }
ifndef          { BEGIN(pp_ifndef); }
define          { BEGIN(pp_define); }
undef           { BEGIN(pp_undef); }
else            {
                    ParserContext* ctx = yyget_extra( yyscanner );
                    if( ctx->m_PPCurrScope == 0 )
                    {
                        yyerror( yyget_extra(yyscanner), yyscanner, "mis-matching #ifdef/#else/#endif" ); 
                        BEGIN(INITIAL);
                        yyterminate(); 
                    }
                    ctx->m_PPUntilScope = ctx->m_PPCurrScope;
                    BEGIN(pp_eat_until); 
                }
endif           {
                    ParserContext* ctx = yyget_extra( yyscanner );
                    if( ctx->m_PPCurrScope == 0 )
                    {
                        yyerror( yyget_extra(yyscanner), yyscanner, "mis-matching #ifdef/#else/#endif" ); 
                        BEGIN(INITIAL);
                        yyterminate(); 
                    }
                    ctx->m_PPCurrScope--; 
                    BEGIN(INITIAL);
                }
include         { BEGIN(pp_include); }
{NEWLINE}       { 
                    yyerror( yyget_extra(yyscanner), yyscanner, yyget_lineno(yyscanner) - 1, "invalid pre-processor directive" );
                    BEGIN(INITIAL);
                }
.               { BEGIN(pp_eat_statement); yyless(0); }
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file in pre-processor directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}

<pp_include>{
{WHITESPACE}    { /* eat whitespace */ }
{PPINCSTART}    { BEGIN(pp_inc_string); }
.               {   
                    yyerror( yyget_extra(yyscanner), yyscanner, "invalid include statement" ); 
                    BEGIN(INITIAL);
                    yyterminate();
                }
}

<pp_inc_string>{
{PPINCSTOP}     {   /*closing quote*/
                    /* setup the buffer here */
                    ParserContext* ctx = yyget_extra( yyscanner );
                    push_parse_file( ctx, ctx->m_TmpString.c_str() );
                    ctx->m_TmpString = "";
                    BEGIN(INITIAL);
                }
\\              { yyget_extra(yyscanner)->m_TmpString += '/'; }
\\\"            { yyget_extra(yyscanner)->m_TmpString += '"'; }

{NEWLINE}       { 
                    yyerror( yyget_extra(yyscanner), yyscanner, yyget_lineno(yyscanner) - 1, "include strings cannot span multiple lines" ); 
                    BEGIN(INITIAL);
                    yyterminate();  
                }
[^\\\n\">]+     { yyget_extra(yyscanner)->m_TmpString += yytext; }
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file in include string" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}

<pp_eat_statement>{
[^\\\n]+        { yyget_extra( yyscanner )->m_TmpString += yytext; }
\\              { yyget_extra( yyscanner )->m_NewLines++; yyget_extra( yyscanner )->m_TmpString += yytext; }
\n              { 
                    ParserContext* ctx = yyget_extra( yyscanner );
                    if( ctx->m_NewLines == 0 )
                    {
                        char tmp[2048];
                        sprintf( tmp, "unknown preprocessor directive:\n\t#%s", ctx->m_TmpString.c_str() );
                        yywarning( ctx, yyscanner, yyget_lineno(yyscanner) - 1, tmp );
                        ctx->m_TmpString = "";
                        BEGIN(INITIAL);
                    } 
                    else
                    {
                        ctx->m_TmpString += '\n';
                        ctx->m_TmpString += '\t';
                        ctx->m_NewLines--;
                    }
                }
<<EOF>>         { 
                    ParserContext* ctx = yyget_extra( yyscanner );
                    char tmp[2048];
                    sprintf( tmp, "unknown preprocessor directive:\n\t#%s", ctx->m_TmpString.c_str() );
                    yywarning( ctx, yyscanner, tmp );
                    ctx->m_TmpString    = "";
                    ctx->m_NewLines     = 0;
                    BEGIN(INITIAL);
                    EOF_DANCE;
                }
}                   //end of the <pp_eat_statement> family of rules

<pp_ifdef>{
[\t\r ]+        { /* eat the white-space */ }
[^\t\r \n]+     { 
                    ParserContext* ctx = yyget_extra( yyscanner );
                    ctx->m_PPCurrScope++;
                    if( ctx->m_Tree->IsDefined( yytext ) )
                    {
                        BEGIN(INITIAL);                     
                    }
                    else
                    {
                        ctx->m_PPUntilScope = ctx->m_PPCurrScope - 1;
                        BEGIN(pp_eat_until);
                    }
                }
\n              {
                    yyerror( yyget_extra(yyscanner), yyscanner, "unexpected newline in #ifdef directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }               
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file #ifdef directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}

<pp_ifndef>{
[\t\r ]+        { /* eat the white-space */ }
[^\t\r \n]+     { 
                    ParserContext* ctx = yyget_extra( yyscanner );
                    ctx->m_PPCurrScope++;
                    if( ctx->m_Tree->IsDefined( yytext ) )
                    {
                        ctx->m_PPUntilScope = ctx->m_PPCurrScope - 1;
                        BEGIN(pp_eat_until);
                    }
                    else
                    {
                        BEGIN(INITIAL);
                    }
                }
\n              {
                    yyerror( yyget_extra(yyscanner), yyscanner, "unexpected newline in #ifndef directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }               
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file #ifndef directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}
<pp_define>{
[\t\r ]+        { /* eat the white-space */ }
[^\t\r \n]+     { 
                    ParserContext* ctx = yyget_extra( yyscanner );
                    ctx->m_Tree->Define( yytext );
                    BEGIN(INITIAL);
                }
\n              {
                    yyerror( yyget_extra(yyscanner), yyscanner, "unexpected newline in #define directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }               
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file #define directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}
<pp_undef>{
[\t\r ]+        { /* eat the white-space */ }
[^\t\r \n]+     { 
                    ParserContext* ctx = yyget_extra( yyscanner );
                    ctx->m_Tree->Undefine( yytext );
                    BEGIN(INITIAL);
                }
\n              {
                    yyerror( yyget_extra(yyscanner), yyscanner, "unexpected newline in #undef directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }               
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "end of file #undef directive" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}

<pp_eat_until>{
#ifndef |
#ifdef  |
#if             {
                    ParserContext* ctx = yyget_extra( yyscanner );
                    ctx->m_PPCurrScope++;
                }
#else           {
                    ParserContext* ctx = yyget_extra( yyscanner );
                    if( ctx->m_PPCurrScope == ctx->m_PPUntilScope + 1 )
                        BEGIN(INITIAL);
                }
#endif          {
                    ParserContext* ctx = yyget_extra( yyscanner );
                    ctx->m_PPCurrScope--;
                    if( ctx->m_PPCurrScope == ctx->m_PPUntilScope )
                        BEGIN(INITIAL);
                }
[^#\n]+         { /* just eat it */ }
\n+             { /* just eat it */ }
#+              { /* just eat it */ }
<<EOF>>         { 
                    yyerror( yyget_extra(yyscanner), yyscanner, "#ifdef/#ifndef statements cannot span several files" ); 
                    BEGIN(INITIAL);
                    yyterminate(); 
                }
}

%%

int fill_lex_buffer( ParserContext* ctx, char* buffer, int maxsize )
{
    if( !ctx->m_File )
        return 0;
        
    if( feof( ctx->m_File->m_File ) )
        return 0;
    
    size_t read = fread( buffer, 1, maxsize, ctx->m_File->m_File );
    return read;
}

bool push_parse_file( ParserContext* ctx, const char* filename )
{
    int len = strlen( filename );
    if( len > 2047 )
        return false;

    ParseFile* file = ctx->m_Tree->CreateParseFile();
	if( ctx->m_File )
	{
#ifdef WIN32
		char slash = '\\';
#else
		char slash = '/';
#endif
		int s = 0, last = -1;
		char* p = ctx->m_File->m_Name;
		while( p && *p )
		{
			if( *p == slash )
				last = s;
			++p; ++s;	
		} 
		if( last != -1 )
		{
			memcpy( file->m_Name, ctx->m_File->m_Name, last+1 );
			memcpy( &file->m_Name[last+1], filename, len+1 );
		}
		else
		{
			memcpy( file->m_Name, filename, len+1 );
		}
	}
	else
	{
	    memcpy( file->m_Name, filename, len+1 );
	}
    file->m_File    = fopen( file->m_Name, "r" );
    file->m_Buffer  = 0x0;
    file->m_Next    = ctx->m_File;
    
    if( !file->m_File )
    {
        char tmp[4096];
        sprintf( tmp, "unable to open input file \"%s\"", filename );
        yyerror( ctx, ctx->yyscanner, tmp );
        ctx->m_Tree->FreeParseFile( file );
        return false;
    }

    file->m_Buffer = yy_create_buffer( file->m_File, YY_BUF_SIZE, ctx->yyscanner );
    yy_switch_to_buffer( file->m_Buffer, ctx->yyscanner );  

    ctx->m_File = file;

    return true;
}

bool pop_parse_file( ParserContext* ctx )
{
    if( !ctx || !ctx->m_File )
        return true;
        
    ParseFile* file = ctx->m_File;
    ctx->m_File     = file->m_Next;

    if( ctx->m_File )
        yy_switch_to_buffer( ctx->m_File->m_Buffer, ctx->yyscanner );

    fclose( file->m_File );
    yy_delete_buffer( file->m_Buffer, ctx->yyscanner );

    ctx->m_Tree->FreeParseFile( file );

    return ctx->m_File == 0x0 ? true : false;   
}


