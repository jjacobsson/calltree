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

class CbInst<bits<8> op, string asm, dag outs, dag ins> : Instruction {
  let AsmString = asm;
  let Namespace = "CB";
  
  dag OutOperandList = outs;
  dag InOperandList = ins;
  
  field bits<32> Inst;
  let Inst{31-24} = op;
}

// 0 destination, 0 argument
class F00<bits<8> op, string asm> : CbInst<op, asm, (outs), (ins)> {
}

// 0 destination, 1 argument
class F01<bits<8> op, string asm> : CbInst<op, asm, (outs), (ins CbRegs:$a)> {
	bits<16> zero = 0;
	bits<8> arg;
	let Inst{23-16} = arg;
	let Inst{15-0}  = zero;
}

// 1 destination, 1 argument
class F11<bits<8> op, string asm, list<dag> pattern> : CbInst<op, asm, (outs CbRegs:$dst), (ins CbRegs:$a)> {
	bits<8> zero = 0;
	bits<8> d1;
	bits<8> a1;
	let Inst{23-16} = d1;
	let Inst{15-8}  = a1;
	let Inst{7-0}   = zero;
	let Pattern = pattern;
}

// 1 destination, 2 argument
class F12<bits<8> op, string asm, list<dag> pattern> : CbInst<op, asm, (outs CbRegs:$dst), (ins CbRegs:$a, CbRegs:$b)> {
	bits<8> d1;
	bits<8> a1;
	bits<8> a2;
	let Inst{23-16} = d1;
	let Inst{15-8}  = a1;
	let Inst{7-0}   = a2;
	let Pattern = pattern;
}






