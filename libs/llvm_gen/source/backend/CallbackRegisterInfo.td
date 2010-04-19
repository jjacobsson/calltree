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
  field bits<8> Num;
  let Num = num;
  let Namespace = "CB";
}

def R_r0  : Reg<  0, "r0">,  DwarfRegNum<[0]>;
def R_r1  : Reg<  1, "r1">,  DwarfRegNum<[1]>;
def R_r2  : Reg<  2, "r2">,  DwarfRegNum<[2]>;
def R_r3  : Reg<  3, "r3">,  DwarfRegNum<[3]>;
def R_r4  : Reg<  4, "r4">,  DwarfRegNum<[4]>;
def R_r5  : Reg<  5, "r5">,  DwarfRegNum<[5]>;
def R_r6  : Reg<  6, "r6">,  DwarfRegNum<[6]>;
def R_r7  : Reg<  7, "r7">,  DwarfRegNum<[7]>;
def R_r8  : Reg<  8, "r8">,  DwarfRegNum<[8]>;
def R_r9  : Reg<  9, "r9">,  DwarfRegNum<[9]>;
def R_r10 : Reg< 10, "r10">, DwarfRegNum<[10]>;
def R_r11 : Reg< 11, "r11">, DwarfRegNum<[11]>;
def R_r12 : Reg< 12, "r12">, DwarfRegNum<[12]>;
def R_r13 : Reg< 13, "r13">, DwarfRegNum<[13]>;
def R_r14 : Reg< 14, "r14">, DwarfRegNum<[14]>;
def R_r15 : Reg< 15, "r15">, DwarfRegNum<[15]>;
def R_r16 : Reg< 16, "r16">, DwarfRegNum<[16]>;
def R_r17 : Reg< 17, "r17">, DwarfRegNum<[17]>;
def R_r18 : Reg< 18, "r18">, DwarfRegNum<[18]>;
def R_r19 : Reg< 19, "r19">, DwarfRegNum<[19]>;
def R_r20 : Reg< 20, "r20">, DwarfRegNum<[20]>;
def R_r21 : Reg< 21, "r21">, DwarfRegNum<[21]>;
def R_rr  : Reg< 22, "rr">,  DwarfRegNum<[22]>;
def R_fc  : Reg< 23, "fc">,  DwarfRegNum<[23]>;
def R_ic  : Reg< 24, "ic">,  DwarfRegNum<[24]>;
def R_ip  : Reg< 25, "ip">,  DwarfRegNum<[25]>;
def R_sp  : Reg< 26, "sp">,  DwarfRegNum<[26]>;
def R_fs  : Reg< 27, "fs">,  DwarfRegNum<[27]>;
def R_ds  : Reg< 28, "ds">,  DwarfRegNum<[28]>;
def R_ms  : Reg< 29, "ms">,  DwarfRegNum<[29]>;
def R_ft  : Reg< 30, "ft">,  DwarfRegNum<[30]>;
def R_jt  : Reg< 31, "jt">,  DwarfRegNum<[31]>;

def CbRegs : RegisterClass<"CB", [i32], 32, 
    [ R_r0, R_r1, R_r2, R_r3, R_r4, R_r5, R_r6, R_r7, R_r8, R_r9,
      R_r10, R_r11, R_r12, R_r13, R_r14, R_r15, R_r16, R_r17,
      R_r18, R_r19, R_r20, R_r21,
      R_rr, R_fc, R_ic, R_ip, R_sp, R_fs, R_ds, R_ms, R_ft, R_jt ]> {
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
