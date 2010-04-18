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

#include <llvm_gen/llvm_gen.h>

#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/PassManager.h>
#include <llvm/CallingConv.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace llvm_gen
{

void init( Program* p )
{
  p->m_Module = new Module( "test", getGlobalContext() );
}

void destroy( Program* p )
{
  delete p->m_Module;
}

void generate( BehaviorTreeContext btc, Program* p )
{
  Module* mod = p->m_Module;

  Constant* c = mod->getOrInsertFunction("mul_add",
  /*ret type*/                           IntegerType::get(getGlobalContext(),32),
  /*args*/                               IntegerType::get(getGlobalContext(),32),
                                         IntegerType::get(getGlobalContext(),32),
                                         IntegerType::get(getGlobalContext(),32),
  /*varargs terminated with null*/       NULL);

  Function* mul_add = cast<Function>(c);
  mul_add->setCallingConv(CallingConv::C);

  Function::arg_iterator args = mul_add->arg_begin();
  Value* x = args++;
  x->setName("x");
  Value* y = args++;
  y->setName("y");
  Value* z = args++;
  z->setName("z");

  BasicBlock* block = BasicBlock::Create(getGlobalContext(), "entry", mul_add);
  IRBuilder<> builder(block);

  Value* tmp = builder.CreateBinOp(Instruction::Mul,
                                   x, y, "tmp");
  Value* tmp2 = builder.CreateBinOp(Instruction::Add,
                                    tmp, z, "tmp2");

  builder.CreateRet(tmp2);

  //PassManager PM;
  //PM.add(createPrintModulePass(&outs()));
  //PM.run(*mod);
}

}
