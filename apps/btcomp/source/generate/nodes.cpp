/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "nodes.h"
#include "program.h"

#include <btree/btree_data.h>

int gen_con( Node* n, Program* p )
{
	if( !n->m_Declared )
		return -1;

	switch( n->m_Grist.m_Type )
	{
	case E_GRIST_SEQUENCE:
		return gen_con_sequence( n, p );
		break;
	case E_GRIST_SELECTOR:
		return gen_con_selector( n, p );
		break;
	case E_GRIST_PARALLEL:
		return gen_con_parallel( n, p );
		break;
	case E_GRIST_DYN_SELECTOR:
		return gen_con_dynselector( n, p );
		break;
	case E_GRIST_DECORATOR:
		return gen_con_decorator( n, p );
		break;
	case E_GRIST_ACTION:
		return gen_con_action( n, p );
		break;
	}
	return -1;
}

int gen_exe( Node* n, Program* p )
{
	if( !n->m_Declared )
		return -1;

	switch( n->m_Grist.m_Type )
	{
	case E_GRIST_SEQUENCE:
		return gen_exe_sequence( n, p );
		break;
	case E_GRIST_SELECTOR:
		return gen_exe_selector( n, p );
		break;
	case E_GRIST_PARALLEL:
		return gen_exe_parallel( n, p );
		break;
	case E_GRIST_DYN_SELECTOR:
		return gen_exe_dynselector( n, p );
		break;
	case E_GRIST_DECORATOR:
		return gen_exe_decorator( n, p );
		break;
	case E_GRIST_ACTION:
		return gen_exe_action( n, p );
		break;
	}
	return -1;
}

int gen_des( Node* n, Program* p )
{
	if( !n->m_Declared )
		return -1;

	switch( n->m_Grist.m_Type )
	{
	case E_GRIST_SEQUENCE:
		return gen_des_sequence( n, p );
		break;
	case E_GRIST_SELECTOR:
		return gen_des_selector( n, p );
		break;
	case E_GRIST_PARALLEL:
		return gen_des_parallel( n, p );
		break;
	case E_GRIST_DYN_SELECTOR:
		return gen_des_dynselector( n, p );
		break;
	case E_GRIST_DECORATOR:
		return gen_des_decorator( n, p );
		break;
	case E_GRIST_ACTION:
		return gen_des_action( n, p );
		break;
	}
	return -1;
}

int gen_con_sequence( Node* n, Program* p )
{
	return -1;
}

int gen_exe_sequence( Node* n, Program* p )
{
	return -1;
}

int gen_des_sequence( Node* n, Program* p )
{
	return -1;
}

int gen_con_selector( Node* n, Program* p )
{
	return -1;
}

int gen_exe_selector( Node* n, Program* p )
{
	return -1;
}

int gen_des_selector( Node* n, Program* p )
{
	return -1;
}

int gen_con_parallel( Node* n, Program* p )
{
	return -1;
}

int gen_exe_parallel( Node* n, Program* p )
{
	return -1;
}

int gen_des_parallel( Node* n, Program* p )
{
	return -1;
}

int gen_con_dynselector( Node* n, Program* p )
{
	return -1;
}

int gen_exe_dynselector( Node* n, Program* p )
{
	return -1;
}

int gen_des_dynselector( Node* n, Program* p )
{
	return -1;
}

int gen_con_decorator( Node* n, Program* p )
{
	return -1;
}

int gen_exe_decorator( Node* n, Program* p )
{
	return -1;
}

int gen_des_decorator( Node* n, Program* p )
{
	return -1;
}

int gen_con_action( Node* n, Program* p )
{
	return -1;
}

int gen_exe_action( Node* n, Program* p )
{
	return -1;
}

int gen_des_action( Node* n, Program* p )
{
	return -1;
}
