/*******************************************************************************
 * Copyright (c) 2010-04-20 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "CallbackInstrInfo.h"
#include "CallbackInstrInfo.inc"

namespace llvm
{

CallbackInstrInfo::CallbackInstrInfo()
  : TargetInstrInfoImpl( CallbackInsts, array_lengthof(CallbackInsts) )
  , m_RI( *this )
{
}

}
