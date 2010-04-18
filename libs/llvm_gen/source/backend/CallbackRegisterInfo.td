//*******************************************************************************
// * Copyright (c) 2010-04-16 Joacim Jacobsson.
// * All rights reserved. This program and the accompanying materials
// * are made available under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available at
// * http://www.eclipse.org/legal/epl-v10.html
// *
// * Contributors:
// *    Joacim Jacobsson - first implementation
// *******************************************************************************/

class Reg<bits<8> num, string n> : Register<n> {
  let Namespace = "llvm_gen";
  field bits<8> Num;
  let Num = num;
}

def r0  : Reg<  0, "r0">,  DwarfRegNum<[0]>;
def r1  : Reg<  1, "r1">,  DwarfRegNum<[1]>;
def r2  : Reg<  2, "r2">,  DwarfRegNum<[2]>;
def r3  : Reg<  3, "r3">,  DwarfRegNum<[3]>;
def r4  : Reg<  4, "r4">,  DwarfRegNum<[4]>;
def r5  : Reg<  5, "r5">,  DwarfRegNum<[5]>;
def r6  : Reg<  6, "r6">,  DwarfRegNum<[6]>;
def r7  : Reg<  7, "r7">,  DwarfRegNum<[7]>;
def r8  : Reg<  8, "r8">,  DwarfRegNum<[8]>;
def r9  : Reg<  9, "r9">,  DwarfRegNum<[9]>;
def r10 : Reg< 10, "r10">, DwarfRegNum<[10]>;
def r11 : Reg< 11, "r11">, DwarfRegNum<[11]>;
def r12 : Reg< 12, "r12">, DwarfRegNum<[12]>;
def r13 : Reg< 13, "r13">, DwarfRegNum<[13]>;
def r14 : Reg< 14, "r14">, DwarfRegNum<[14]>;
def r15 : Reg< 15, "r15">, DwarfRegNum<[15]>;
def r16 : Reg< 16, "r16">, DwarfRegNum<[16]>;
def r17 : Reg< 17, "r17">, DwarfRegNum<[17]>;
def r18 : Reg< 18, "r18">, DwarfRegNum<[18]>;
def r19 : Reg< 19, "r19">, DwarfRegNum<[19]>;
def r20 : Reg< 20, "r20">, DwarfRegNum<[20]>;
def r21 : Reg< 21, "r21">, DwarfRegNum<[21]>;
def rr  : Reg< 22, "rr">,  DwarfRegNum<[22]>;
def fc  : Reg< 23, "fc">,  DwarfRegNum<[23]>;
def ic  : Reg< 24, "ic">,  DwarfRegNum<[24]>;
def ip  : Reg< 25, "ip">,  DwarfRegNum<[25]>;
def sp  : Reg< 26, "sp">,  DwarfRegNum<[26]>;
def fs  : Reg< 27, "fs">,  DwarfRegNum<[27]>;
def ds  : Reg< 28, "ds">,  DwarfRegNum<[28]>;
def ms  : Reg< 29, "ms">,  DwarfRegNum<[29]>;
def ft  : Reg< 30, "ft">,  DwarfRegNum<[30]>;
def jt  : Reg< 31, "jt">,  DwarfRegNum<[31]>;

def RegClass : RegisterClass<"llvm_gen", [i32], 32, 
    [ r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
      rr, fc, ic, ip, sp, fs, ds, ms, ft, jt ]> {
  let MethodProtos = [{
    iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  let MethodBodies = [{
    IntRegsClass::iterator
    IntRegsClass::allocation_order_end(const MachineFunction &MF) const {
      return end() - 10  // Don't allocate special registers
         -1;
    }
  }];
} 
