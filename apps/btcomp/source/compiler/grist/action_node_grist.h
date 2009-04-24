/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#ifndef ACTION_GRIST_H_
#define ACTION_GRIST_H_


#include "../variable_owner_helper.h"

class Action;

class ActionNodeGrist : public VariableOwner
{
public:
	ActionNodeGrist();
	~ActionNodeGrist();

	int GetType() const { return E_GRIST_ACTION; }

	void GenerateConstructionCode( BehaviourTree* bt );
	void GenerateExecutionCode( BehaviourTree* bt );
	void GenerateDestructionCode( BehaviourTree* bt );

	void SetAction( Action* action );
	Action* GetAction();


private:

	Action*	m_Action;
	int		m_bssPos;
};

#endif /*ACTION_GRIST_H_*/
