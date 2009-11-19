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

%x ml_comment
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
dyn_selector    { return T_DSELECTOR; }
parallel        { return T_PARALLEL; }
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
#.*             { /* eat single line comments */ }

\"              { BEGIN(string); }
"/*"            { BEGIN(ml_comment); }

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
[^\\\n\"]+      { yyget_extra(yyscanner)->m_TmpString += yytext; }

([\n])			{ yyerror( yyget_extra(yyscanner), yyscanner, "new-line in string." ); }

}

<ml_comment>{
[^*\n]*         { }
[^*\n]*\n       { }
"*"+[^*/\n]*    { }
"*"+[^*/\n]*\n  { }
"*"+"/"         { BEGIN(INITIAL); }
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


