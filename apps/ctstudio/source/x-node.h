/*******************************************************************************
 * Copyright (c) 2009-04-26 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef XNODE_H_INCLUDED
#define XNODE_H_INCLUDED

enum XNodeDataTypes
{
  E_XNDT_TREE,
  E_XNDT_NODE
};

struct XNodeData
{
  XNodeDataTypes m_Type;
  int m_NodeGrist;
  int m_FuncId;
};


#endif /* XNODE_H_ */
