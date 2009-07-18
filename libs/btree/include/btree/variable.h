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

struct Variable
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
    Identifier*    m_Id;
    UData           m_Data;
};


class VariableList
{
public:

    VariableList();
    ~VariableList();

    static const int s_MaxVariableCount = 128;

    typedef Variable**                 iterator;
    typedef const Variable* const *    const_iterator;

    inline iterator begin()                         { return &m_Variables[0]; }
    inline iterator end()                           { return &m_Variables[m_Count]; }

    inline const_iterator begin() const             { return &m_Variables[0]; }
    inline const_iterator end() const               { return &m_Variables[m_Count]; }

    inline Variable* operator[] (int i)                { return m_Variables[i]; }
    inline const Variable* operator[] (int i) const    { return m_Variables[i]; }

    void SetTree( BehaviourTree* bt );
    bool Append( Variable* variable );
    int Size();

private:
    BehaviourTree* m_Tree;
    Variable*     m_Variables[ s_MaxVariableCount ];
    int            m_Count;
};

struct VariableSortPred
{
    bool operator() ( Variable* lhs, Variable* rhs ) const;
};

#endif /*VARIABLE_H_*/
