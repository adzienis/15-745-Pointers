#ifndef andersenresult_hpp_
#define andersenresult_hpp_

#include "llvm/Analysis/CallGraph.h"
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/SymbolTableListTraits.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <llvm/Support/Casting.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <set>
#include <unordered_map>
#include "andersen.hpp"

namespace llvm {

class FunctionInfo : public ModulePass, AliasAnalysis {
public:
  static char ID;

  AndersenAAResult rest;

  FunctionInfo() : ModulePass(ID), AliasAnalysis(rest) {}
  ~FunctionInfo() {}

  bool runOnModule(Module &M) override;

  void andersen(Function *F, int depth);

  std::unordered_map<Value *, std::set<Value *>> points_to;
};
}; // namespace llvm

#endif