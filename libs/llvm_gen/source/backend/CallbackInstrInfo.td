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

def I_nop   : CbInst<"nop",   (outs), (ins) >;
def I_call  : CbInst<"call",  (outs), (ins) >;
def I_ret   : CbInst<"ret",   (outs), (ins) >;
def I_ccal  : CbInst<"ccall", (outs), (ins) >;
def I_dcall : CbInst<"dcall", (outs), (ins) >;
def I_br    : CbInst<"br",    (outs), (ins) >;
def I_bre   : CbInst<"bre",   (outs), (ins) >;
def I_brne  : CbInst<"brne",  (outs), (ins) >;
def I_mov   : CbInst<"mov",   (outs), (ins) >;
def I_add   : CbInst<"add",   (outs), (ins) >;
def I_sub   : CbInst<"sub",   (outs), (ins) >;
def I_mul   : CbInst<"mul",   (outs), (ins) >;
def I_div   : CbInst<"div",   (outs), (ins) >;
def I_addi  : CbInst<"addi",  (outs), (ins) >;
def I_subi  : CbInst<"subi",  (outs), (ins) >;
def I_muli  : CbInst<"muli",  (outs), (ins) >;
def I_divi  : CbInst<"divi",  (outs), (ins) >;
def I_ld    : CbInst<"ld",    (outs), (ins) >;
def I_st    : CbInst<"st",    (outs), (ins) >;
def I_push  : CbInst<"push",  (outs), (ins) >;
def I_pop   : CbInst<"pop",   (outs), (ins) >;
def I_shli  : CbInst<"shli",  (outs), (ins) >;
def I_shri  : CbInst<"shri",  (outs), (ins) >;
def I_and   : CbInst<"and",   (outs), (ins) >;
def I_or    : CbInst<"or",    (outs), (ins) >;
def I_xor   : CbInst<"xor",   (outs), (ins) >;
def I_slszi : CbInst<"slszi", (outs), (ins) >;
def I_shszi : CbInst<"shszi", (outs), (ins) >;
def I_andli : CbInst<"andli", (outs), (ins) >;
def I_andhi : CbInst<"andhi", (outs), (ins) >;
def I_orli  : CbInst<"orli",  (outs), (ins) >; 
def I_orhi  : CbInst<"orhi",  (outs), (ins) >;
def I_xorli : CbInst<"xorli", (outs), (ins) >; 
def I_xorhi : CbInst<"xorhi", (outs), (ins) >;
def I_exit  : CbInst<"exit",  (outs), (ins) >;

