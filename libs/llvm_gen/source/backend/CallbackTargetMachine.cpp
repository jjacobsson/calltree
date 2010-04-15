/*******************************************************************************
 * Copyright (c) 2010-04-15 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "CallbackTargetMachine.h"

#include <llvm/PassManager.h>
#include <llvm/Target/TargetRegistry.h>

using namespace llvm_gen;
using namespace llvm;

Target TheCallbackTarget;

extern "C" void LLVMInitializeCallbackTarget() {
  // Register the target.
  RegisterTargetMachine<CallbackTargetMachine> X(TheCallbackTarget);
  //RegisterAsmInfo<SparcELFTargetAsmInfo> Y(TheCallbackTarget);

}


CallbackTargetMachine::CallbackTargetMachine(const Target &t, const std::string &tt,
  const std::string &fs )
  : LLVMTargetMachine( t, tt )
  , DataLayout( "E-p:32:32-f32:32:32" )
  //, Subtarget( M, FS )
  //, InstrInfo( Subtarget )
  , FrameInfo( TargetFrameInfo::StackGrowsDown, 8, 0 )
{

}
