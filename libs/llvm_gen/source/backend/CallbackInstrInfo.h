/*******************************************************************************
 * Copyright (c) 2010-04-16 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/


#ifndef CALLBACKINSTRUCTIONINFO_H_INCLUDED
#define CALLBACKINSTRUCTIONINFO_H_INCLUDED

#include <llvm/Target/TargetInstrInfo.h>
#include "CallbackRegisterInfo.h"

namespace llvm
{

class CallbackInstrInfo : public TargetInstrInfoImpl
{
public:
  CallbackInstrInfo();
private:
  const CallbackRegisterInfo m_RI;
};

}

#endif /* CALLBACKINSTRUCTIONINFO_H_INCLUDED */
