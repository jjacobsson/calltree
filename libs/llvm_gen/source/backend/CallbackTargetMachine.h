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


#ifndef CALLBACKTARGETMACHINE_H_INCLUDED
#define CALLBACKTARGETMACHINE_H_INCLUDED


#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetFrameInfo.h>

#include "CallbackInstrInfo.h"

namespace llvm {

class CallbackTargetMachine : public LLVMTargetMachine
{
  const TargetData DataLayout;       // Calculates type size & alignment
//  SparcSubtarget Subtarget;
//  SparcInstrInfo InstrInfo;
  TargetFrameInfo FrameInfo;

protected:
  virtual const TargetAsmInfo *createTargetAsmInfo() const;

public:
  CallbackTargetMachine(const Target &T, const std::string &TT,
    const std::string &FS);

/*
  virtual const SparcInstrInfo *getInstrInfo() const {return &InstrInfo; }
  virtual const TargetFrameInfo *getFrameInfo() const {return &FrameInfo; }
  virtual const TargetSubtarget *getSubtargetImpl() const{return &Subtarget; }
  virtual const TargetRegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }
*/
  virtual const TargetData *getTargetData() const { return &DataLayout; }
  static unsigned getModuleMatchQuality(const Module &M);

  // Pass Pipeline Configuration
  virtual bool addInstSelector(PassManagerBase &PM, bool Fast);
  virtual bool addPreEmitPass(PassManagerBase &PM, bool Fast);
  virtual bool addAssemblyEmitter(PassManagerBase &PM, bool Fast,
                                  std::ostream &Out);

};

}

#endif /* CALLBACKTARGETMACHINE_H_INCLUDED */
