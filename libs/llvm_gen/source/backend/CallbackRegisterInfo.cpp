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

#include "CallbackRegisterInfo.h"

namespace llvm
{

CallbackRegisterInfo::CallbackRegisterInfo( const TargetInstrInfo& tii )
  : CallbackGenRegisterInfo( -1, -1 )
  , m_TII( tii )
{
}


}

