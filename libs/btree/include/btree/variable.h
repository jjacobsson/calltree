/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <string>
#include <vector>

#include "identifier.h"

struct Identifier;
class BehaviourTree;

class Variable
{
public:
    enum EVariableTypes
    {
        E_VART_UNDEFINED,
        E_VART_INTEGER,
        E_VART_FLOAT,
        E_VART_STRING,
        E_VART_BOOL
    };

    union UData {
        int         m_Integer;
    	float       m_Float;
        const char* m_String;
        bool		m_Bool;
    };

    bool        SafeToConvert( int to_type ) const;
    int         ValueAsInteger() const;
    float       ValueAsFloat() const;
    const char* ValueAsString() const;
    bool		ValueAsBool() const;

    int        m_Type;
    Identifier m_Id;
    UData      m_Data;
    Variable*  m_Next;
};

void InitVariable( Variable* v );

void DeleteVariableList( Variable* v );

Variable* FindLastVariable( Variable* v );

void AppendToEndOfList( Variable* start, Variable* v );

int CountElementsInList( Variable* start );

Variable* FindVariableWithIdHash( Variable* start, int hash );

int CountOccourancesOfIdHashInList( Variable* start, int hash );

bool VariableIdsAreUniqueInList( Variable* start );

#endif /*VARIABLE_H_*/
