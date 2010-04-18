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

class CbInst<string asmStr> : Instruction {
  let AsmString = asmStr;
  let Namespace = "CB";
  field bits<32> Inst;
  bits<8> Op;
  bits<8> A1;
  bits<8> A2;
  bits<8> A3;
  
  let Inst{31-24} = Op;
  let Inst{23-16} = A1;
  let Inst{15-8} = A2;
  let Inst{7-0} = A3;
}