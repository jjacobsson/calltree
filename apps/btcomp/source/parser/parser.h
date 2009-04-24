/* A Bison parser, made by GNU Bison 2.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_ROOT = 258,
     T_NODE = 259,
     T_ID = 260,
     T_COLON = 261,
     T_SEMICOLON = 262,
     T_COMMA = 263,
     T_ASSIGNMENT = 264,
     T_SEQUENCE = 265,
     T_SELECTOR = 266,
     T_DSELECTOR = 267,
     T_PARALLEL = 268,
     T_ACTION = 269,
     T_DECORATOR = 270,
     T_INT32 = 271,
     T_BOOL = 272,
     T_FLOAT = 273,
     T_STRING = 274,
     T_INT32_VALUE = 275,
     T_BOOL_VALUE = 276,
     T_FLOAT_VALUE = 277,
     T_STRING_VALUE = 278,
     T_END_OF_FILE = 279
   };
#endif
/* Tokens.  */
#define T_ROOT 258
#define T_NODE 259
#define T_ID 260
#define T_COLON 261
#define T_SEMICOLON 262
#define T_COMMA 263
#define T_ASSIGNMENT 264
#define T_SEQUENCE 265
#define T_SELECTOR 266
#define T_DSELECTOR 267
#define T_PARALLEL 268
#define T_ACTION 269
#define T_DECORATOR 270
#define T_INT32 271
#define T_BOOL 272
#define T_FLOAT 273
#define T_STRING 274
#define T_INT32_VALUE 275
#define T_BOOL_VALUE 276
#define T_FLOAT_VALUE 277
#define T_STRING_VALUE 278
#define T_END_OF_FILE 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 50 "parser.y"
{
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
	VariableList*  m_VariableList;
}
/* Line 1488 of yacc.c.  */
#line 112 "parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



