/*******************************************************************************
 * Copyright (c) 2010-04-07 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef CB_GEN_GEN_UTILITY_H_INCLUDED
#define CB_GEN_GEN_UTILITY_H_INCLUDED

#include <cb_gen/cb_gen.h>

namespace cb_gen {

void load_with_offset( InstList& i, uchar to, uchar from, uint o );

void store_with_offset( InstList& i, uchar to, uchar from, uint o );

void set_registry( InstList& i, uchar reg, uint value );

void dressed_call( InstList& i, uint func_id, uint mem_offset );

}

#endif /* CB_GEN_GEN_UTILITY_H_INCLUDED */
