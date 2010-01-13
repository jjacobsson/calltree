/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

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
 * BehaviorTreeContext stuff
 */

BehaviorTreeContext BehaviorTreeContextCreate( BehaviorTreeContextSetup* );

void BehaviorTreeContextDestroy( BehaviorTreeContext );

const char* RegisterString( BehaviorTreeContext, const char* );

const char* RegisterString( BehaviorTreeContext, const char*, hash_t );

void* AllocateObject( BehaviorTreeContext );

void FreeObject( BehaviorTreeContext, void* );

void RegisterSymbol( BehaviorTreeContext, const NamedSymbol& );

NamedSymbol* FindSymbol( BehaviorTreeContext, hash_t );

void AddInclude( BehaviorTreeContext, const char* );

#endif /* BTREE_FUNCTIONS_H_INCLUDED */

