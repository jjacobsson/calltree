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

class Identifier;
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
        float       m_Float;
        int         m_Int;
        const char* m_String;
    };

    bool        SafeToConvert( int to_type ) const;
    int         ValueAsInt() const;
    float       ValueAsFloat() const;
    const char* ValueAsString() const;

    int             m_Type;
    Identifier*     m_Id;
    UData           m_Data;
    Variable*       m_Next;
};

void DeleteVariableList( Variable* v );
Variable* FindLastVariable( Variable* v );

#endif /*VARIABLE_H_*/
