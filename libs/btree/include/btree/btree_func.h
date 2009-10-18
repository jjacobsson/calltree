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

struct Identifier;
struct Variable;
struct Action;
struct Decorator;
struct Node;
struct NodeGrist;

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

const char* ValueAsString( const Variable& );

bool ValueAsBool( const Variable& );

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

void UnlinkFromSiblings( Node *n );

void UnlinkNodeFromParentAndSiblings( Node* n );

/*
 * Node Grist Functions
 */

void InitGrist( NodeGrist* );

#endif /* BTREE_FUNCTIONS_H_INCLUDED */
