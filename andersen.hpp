#ifndef ANDERSEN_HPP_
#define ANDERSEN_HPP_

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

namespace llvm {

class AndersenAAResult : public llvm::AAResultBase<AndersenAAResult> {
private:
  friend llvm::AAResultBase<AndersenAAResult>;

  llvm::AliasResult andersenAlias(const llvm::Value *, const llvm::Value *);

public:
  AndersenAAResult(const llvm::Module &);

  llvm::AliasResult alias(const llvm::MemoryLocation &,
                          const llvm::MemoryLocation &);
  bool pointsToConstantMemory(const llvm::MemoryLocation &, bool);
};


};

#endif