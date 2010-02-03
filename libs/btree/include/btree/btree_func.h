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
 * Identifier Functions
 */

void InitIdentifier( Identifier* id );

/*
 * Variable Functions
 */

void InitVariable( Variable* v );

void DeleteVariableList( Variable* v );

Variable* FindLastVariable( Variable* v );

void AppendToEndOfList( Variable* start, Variable* v );

int CountElementsInList( Variable* start );

Variable* FindVariableWithIdHash( Variable* start, int hash );

int CountOccourancesOfIdHashInList( Variable* start, int hash );

bool VariableIdsAreUniqueInList( Variable* start );

bool SafeToConvert( const Variable&, int to_type );

int ValueAsInteger( const Variable& );

float ValueAsFloat( const Variable& );

const StringData* ValueAsString( const Variable& );

bool ValueAsBool( const Variable& );

const char* GetVariableListAsString( BehaviorTreeContext tree, Variable* start );

/*
 * BehaviorTree Functions
 */

void InitBehaviorTree( BehaviorTree* );

/*
 * Action Functions
 */

void InitAction( Action* );

/*
 * Decorator Functions
 */

void InitDecorator( Decorator* );

/*
 * Node Functions
 */

void InitNode( Node* );

void AppendToEndOfList( Node* s, Node* n );

void SetParentOnChildren( Node* s );

Node* GetFirstChild( Node* n );

void SetFirstChild( Node* p, Node* c );

void UnlinkFromSiblings( Node *n );

void UnlinkNodeFromParentAndSiblings( Node* n );

int CountChildNodes( Node* n );

bool AcceptsMoreChildren( Node* n );

/*
 * Node Grist Functions
 */

void InitGrist( NodeGrist* );

/*
 * BehaviorTreeContext Functions
 */

BehaviorTreeContext BehaviorTreeContextCreate( BehaviorTreeContextSetup* );

void BehaviorTreeContextDestroy( BehaviorTreeContext );

const char* BehaviorTreeContextRegisterString( BehaviorTreeContext, const char* );

const char* BehaviorTreeContextRegisterString( BehaviorTreeContext, const char*, hash_t );

void* BehaviorTreeContextAllocateObject( BehaviorTreeContext );

void BehaviorTreeContextFreeObject( BehaviorTreeContext, void* );

void BehaviorTreeContextRegisterSymbol( BehaviorTreeContext, const NamedSymbol& );

NamedSymbol* BehaviorTreeContextFindSymbol( BehaviorTreeContext, hash_t );

NamedSymbol* BehaviorTreeContextAccessSymbols( BehaviorTreeContext, int* );

void BehaviorTreeContextAddInclude( BehaviorTreeContext, const Include& );

Include* BehaviorTreeContextGetFirstInclude( BehaviorTreeContext );

/*
 * String Buffer Functions
 */

void StringBufferInit( ParserContext, StringBuffer* );
void StringBufferDestroy( ParserContext, StringBuffer* );

void StringBufferAppend( ParserContext, StringBuffer*, char );
void StringBufferAppend( ParserContext, StringBuffer*, const char * );
void StringBufferAppend( ParserContext, StringBuffer*, const char *, int );

void StringBufferClear( ParserContext, StringBuffer* );

void StringBufferGrow( ParserContext, StringBuffer* sb, int min );

/*
 * Parser Functions
 */

ParserContext ParserContextCreate( BehaviorTreeContext );

void ParserContextDestroy( ParserContext );

void* ParserContextGetExtra( ParserContext );
void ParserContextSetExtra( ParserContext, void* );

void ParserContextSetCurrent( ParserContext, const char* );
const char* ParserContextGetCurrent( ParserContext );

int ParserContextGetLineNo( ParserContext );

BehaviorTreeContext ParserContextGetBehaviorTreeContext( ParserContext );

int Parse( ParserContext, ParserContextFunctions* );

#endif /* BTREE_FUNCTIONS_H_INCLUDED */

