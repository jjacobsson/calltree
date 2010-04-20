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

include "CallbackInstrFormats.td"

def I_nop   : F00<0, "nop">;
def I_call  : F01<1, "call">;
def I_ret   : F00<2, "ret">;
def I_ccal  : CbInst<0,"ccall", (outs), (ins) >;
def I_dcall : CbInst<0,"dcall", (outs), (ins) >;
def I_br    : CbInst<0,"br",    (outs), (ins) >;
def I_bre   : CbInst<0,"bre",   (outs), (ins) >;
def I_brne  : CbInst<0,"brne",  (outs), (ins) >;
def I_mov   : F11<8, "mov $dst, $a",      [ (set CbRegs:$dst, CbRegs:$a) ] >;
def I_add   : F12<0, "add $dst, $a, $b",  [ (set CbRegs:$dst, (add CbRegs:$a, CbRegs:$b)) ] >;
def I_sub   : F12<0, "sub $dst, $a, $b",  [ (set CbRegs:$dst, (sub CbRegs:$a, CbRegs:$b)) ] >;
def I_mul   : F12<0, "mul $dst, $a, $b",  [ (set CbRegs:$dst, (mul CbRegs:$a, CbRegs:$b)) ] >;
def I_sdiv  : F12<0, "sdiv $dst, $a, $b", [ (set CbRegs:$dst, (sdiv CbRegs:$a, CbRegs:$b)) ] >;
def I_udiv  : F12<0, "udiv $dst, $a, $b", [ (set CbRegs:$dst, (udiv CbRegs:$a, CbRegs:$b)) ] >;
def I_addi  : CbInst<0,"addi",  (outs), (ins) >;
def I_subi  : CbInst<0,"subi",  (outs), (ins) >;
def I_muli  : CbInst<0,"muli",  (outs), (ins) >;
def I_sdivi : CbInst<0,"sdivi", (outs), (ins) >;
def I_udivi : CbInst<0,"udivi", (outs), (ins) >;
def I_ld    : CbInst<0,"ld",    (outs), (ins) >;
def I_st    : CbInst<0,"st",    (outs), (ins) >;
def I_push  : CbInst<0,"push",  (outs), (ins) >;
def I_pop   : CbInst<0,"pop",   (outs), (ins) >;
def I_shli  : CbInst<0,"shli",  (outs), (ins) >;
def I_shri  : CbInst<0,"shri",  (outs), (ins) >;
def I_and   : F12<0,"and $dst, $a, $b", [ (set CbRegs:$dst, (and CbRegs:$a, CbRegs:$b)) ] >;
def I_or    : F12<0,"or $dst, $a, $b",  [ (set CbRegs:$dst, (or CbRegs:$a, CbRegs:$b)) ] >;
def I_xor   : F12<0,"xor $dst, $a, $b", [ (set CbRegs:$dst, (xor CbRegs:$a, CbRegs:$b)) ] >;
def I_slszi : CbInst<0,"slszi", (outs), (ins) >;
def I_shszi : CbInst<0,"shszi", (outs), (ins) >;
def I_andli : CbInst<0,"andli", (outs), (ins) >;
def I_andhi : CbInst<0,"andhi", (outs), (ins) >;
def I_orli  : CbInst<0,"orli",  (outs), (ins) >; 
def I_orhi  : CbInst<0,"orhi",  (outs), (ins) >;
def I_xorli : CbInst<0,"xorli", (outs), (ins) >; 
def I_xorhi : CbInst<0,"xorhi", (outs), (ins) >;
def I_exit  : F00<34, "exit">;

