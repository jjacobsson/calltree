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

class Reg<string n> : Register<n> {
}

def G0  : Reg<"r0">,  DwarfRegNum<[0]>;
def G1  : Reg<"r1">,  DwarfRegNum<[1]>;
def G2  : Reg<"r2">,  DwarfRegNum<[2]>;
def G3  : Reg<"r3">,  DwarfRegNum<[3]>;
def G4  : Reg<"r4">,  DwarfRegNum<[4]>;
def G5  : Reg<"r5">,  DwarfRegNum<[5]>;
def G6  : Reg<"r6">,  DwarfRegNum<[6]>;
def G7  : Reg<"r7">,  DwarfRegNum<[7]>;
def G8  : Reg<"r8">,  DwarfRegNum<[8]>;
def G9  : Reg<"r9">,  DwarfRegNum<[9]>;
def G10 : Reg<"r10">, DwarfRegNum<[10]>;
def G11 : Reg<"r11">, DwarfRegNum<[11]>;
def G12 : Reg<"r12">, DwarfRegNum<[12]>;
def G13 : Reg<"r13">, DwarfRegNum<[13]>;
def G14 : Reg<"r14">, DwarfRegNum<[14]>;
def G15 : Reg<"r15">, DwarfRegNum<[15]>;
def G16 : Reg<"r16">, DwarfRegNum<[16]>;
def G17 : Reg<"r17">, DwarfRegNum<[17]>;
def G18 : Reg<"r18">, DwarfRegNum<[18]>;
def G19 : Reg<"r19">, DwarfRegNum<[19]>;
def G20 : Reg<"r20">, DwarfRegNum<[20]>;
def G21 : Reg<"r21">, DwarfRegNum<[21]>;
def G22 : Reg<"rr">,  DwarfRegNum<[22]>;
def G23 : Reg<"fc">,  DwarfRegNum<[23]>;
def G24 : Reg<"ic">,  DwarfRegNum<[24]>;
def G25 : Reg<"ip">,  DwarfRegNum<[25]>;
def G26 : Reg<"sp">,  DwarfRegNum<[26]>;
def G27 : Reg<"fs">,  DwarfRegNum<[27]>;
def G28 : Reg<"ds">,  DwarfRegNum<[28]>;
def G29 : Reg<"ms">,  DwarfRegNum<[29]>;
def G30 : Reg<"ft">,  DwarfRegNum<[30]>;
def G31 : Reg<"jt">,  DwarfRegNum<[31]>;

