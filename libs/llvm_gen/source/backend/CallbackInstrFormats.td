//*******************************************************************************
// * Copyright (c) 2010-04-18 Joacim Jacobsson.
// * All rights reserved. This program and the accompanying materials
// * are made available under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available at
// * http://www.eclipse.org/legal/epl-v10.html
// *
// * Contributors:
// *    Joacim Jacobsson - first implementation
// *******************************************************************************/

class CbInst<string asmStr, dag outs, dag ins> : Instruction {
  let AsmString = asmStr;
  let Namespace = "CB";
  
  dag OutOperandList = outs;
  dag InOperandList = ins;
  
  field bits<32> Inst;
  bits<8> Op;
  
  let Inst{31-24} = Op;
}