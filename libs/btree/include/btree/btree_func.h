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

#ifndef BTREE_FUNCTIONS_H_INCLUDED
#define BTREE_FUNCTIONS_H_INCLUDED

#include "btree_data.h"

/*
 * Function that can be used to check if a word or string as one of the keywords
 * used in the btree lexer.
 */

bool is_btree_keyword( const char* str );

/*
 * Identifier Functions
 */

void init( Identifier* id );

void clone( BehaviorTreeContext, Identifier*, Identifier* );

/*
 * Variable Functions
 */

void init( Parameter* v );

Parameter* clone_list( BehaviorTreeContext, Parameter* );

Parameter* find_last( Parameter* v );

void append_to_end( Parameter* start, Parameter* v );

int count_elements( Parameter* start );

Parameter* find_by_hash( Parameter* start, hash_t hash );

int count_occourances_of_hash_in_list( Parameter* start, hash_t hash );

bool id_hashes_are_unique_in_list( Parameter* start );

bool safe_to_convert( const Parameter&, int to_type );

int as_integer( const Parameter& );

float as_float( const Parameter& );

const StringData* as_string( const Parameter& );

bool as_bool( const Parameter& );

const char* list_as_string( BehaviorTreeContext tree, Parameter* start );

/*
 * BehaviorTree Functions
 */

void init( BehaviorTree* );

void set_parent_on_children( BehaviorTree* t );

/*
 * Action Functions
 */

void init( Action* );

/*
 * Decorator Functions
 */

void init( Decorator* );

/*
 * Node Functions
 */

void init( Node* );

void append_to_end( Node* s, Node* n );

void set_parent_on_children( Node* s );

Node* get_first_child( Node* n );

Node* get_first_child( const NodeParent& p );

void set_first_child( Node* p, Node* c );

void set_first_child( const NodeParent& p, Node* c );

void unlink_from_siblings( Node *n );

void unlink_from_parent_and_siblings( Node* n );

int count_children( Node* n );

bool accepts_more_children( Node* n );

Parameter* get_parameters( Node* n );

/*
 * Node Grist Functions
 */

void init( NodeGrist* );

/*
 * BehaviorTreeContext Functions
 */

BehaviorTreeContext create_bt_context( Allocator& );

BehaviorTreeContext clone_bt_context( BehaviorTreeContext );

void destroy( BehaviorTreeContext );

const char* register_string( BehaviorTreeContext, const char* );

const char* register_string( BehaviorTreeContext, const char*, hash_t );

void* allocate_object( BehaviorTreeContext );

void free_object( BehaviorTreeContext, void* );

void register_symbol( BehaviorTreeContext, const NamedSymbol& );

void remove_symbol( BehaviorTreeContext, hash_t );

NamedSymbol* find_symbol( BehaviorTreeContext, hash_t );

NamedSymbol* access_symbols( BehaviorTreeContext, int* );

void add_include( BehaviorTreeContext, const Include& );

Include* create_include( BehaviorTreeContext, const char* );

Include* get_first_include( BehaviorTreeContext );

void release_include( BehaviorTreeContext, Include* );

/*
 * String Buffer Functions
 */

void init( Allocator&, StringBuffer*, int initial_size = 1024 );
void destroy( StringBuffer* );

void append( StringBuffer*, char );
void append( StringBuffer*, const char * );
void append( StringBuffer*, const char *, int );

void clear( StringBuffer* );

void grow( StringBuffer* sb, int min );

/*
 * Parser Functions
 */

ParserContext create_parser_context( BehaviorTreeContext );

void destroy( ParserContext );

void* get_extra( ParserContext );
void set_extra( ParserContext, void* );

void set_current( ParserContext, const char* );
const char* get_current( ParserContext );

int get_line_no( ParserContext );

BehaviorTreeContext get_bt_context( ParserContext );

int parse( ParserContext, ParserContextFunctions* );

/*
 * Saver Functions
 */

SaverContext create_saver_context( BehaviorTreeContext );

void destroy( SaverContext );

void* get_extra( SaverContext );
void set_extra( SaverContext, void* );

BehaviorTreeContext get_bt_context( SaverContext );

void save( SaverContext, SaverContextFunctions* );



#endif /* BTREE_FUNCTIONS_H_INCLUDED */

