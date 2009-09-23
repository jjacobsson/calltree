/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef PARSER_COMMON_H_
#define PARSER_COMMON_H_

#include "../compiler/compiler.h"
#include <btree/btree.h>
#include <btree/identifier.h>
#include "btree_parser.h"
#include <string>

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

struct ParseFile
{
    char            m_Name[2048];
    FILE*           m_File;
    ParseFile*      m_Next;
    YY_BUFFER_STATE m_Buffer;
};

struct ParserContext
{
    ParserContext();

    void*          yyscanner;
    std::string    m_TmpString; // <- argh! kill it! TODO.
    int            m_NewLines;
    BehaviourTree* m_Tree;
    ParseFile*     m_File;
    int            m_PPCurrScope;
    int            m_PPUntilScope;
};

#define YY_EXTRA_TYPE ParserContext*

int  yylex( YYSTYPE*, void* );
int  yylex_init( void** );
int  yylex_destroy( void* );
void yyset_extra( YY_EXTRA_TYPE, void* );
int  yyparse( ParserContext*, void* );
int  yyget_lineno( void* );

int  fill_lex_buffer( ParserContext* ctx, char* buffer, int maxsize );
bool push_parse_file( ParserContext*, const char* filename );
bool pop_parse_file( ParserContext* );

inline void yyerror( ParserContext* ctx, void* scanner, const char* msg )
{
    ctx->m_Tree->Error( ctx, yyget_lineno(scanner), msg );
}

inline void yyerror( ParserContext* ctx, void* scanner, int lineno, const char* msg )
{
    ctx->m_Tree->Error( ctx, lineno, msg );
}

inline void yywarning( ParserContext* ctx, void* scanner, const char* msg )
{
    ctx->m_Tree->Warning( ctx, yyget_lineno(scanner), msg );
}

inline void yywarning( ParserContext* ctx, void* scanner, int lineno, const char* msg )
{
    ctx->m_Tree->Warning( ctx, lineno, msg );
}


#endif /* PARSER_COMMON_H_ */
