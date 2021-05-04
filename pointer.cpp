#include "llvm/Analysis/CallGraph.h"
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/SymbolTableListTraits.h>

#include <algorithm>
#include <functional>
#include <iostream>

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

std::string getShortValueName(Value *v) {
  if (v->getName().str().length() > 0) {
    return "%" + v->getName().str();
  } else if (isa<Instruction>(v)) {
    std::string s = "";
    raw_string_ostream *strm = new raw_string_ostream(s);
    v->print(*strm);
    std::string inst = strm->str();
    size_t idx1 = inst.find("%");
    size_t idx2 = inst.find(" ", idx1);
    if (idx1 != std::string::npos && idx2 != std::string::npos) {
      return inst.substr(idx1, idx2 - idx1);
    } else {
      return "\"" + inst + "\"";
    }
  } else if (ConstantInt *cint = dyn_cast<ConstantInt>(v)) {
    std::string s = "";
    raw_string_ostream *strm = new raw_string_ostream(s);
    cint->getValue().print(*strm, true);
    return strm->str();
  } else if (Argument *arg = dyn_cast<Argument>(v)) {
    std::string s = "";
    raw_string_ostream *strm = new raw_string_ostream(s);
    v->print(*strm);
    std::string inst = strm->str();
    size_t idx1 = inst.find("%");
    if (idx1 != std::string::npos) {
      return inst.substr(idx1);
    } else {
      return "\"" + inst + "\"";
    }

  } else {
    std::string s = "";
    raw_string_ostream *strm = new raw_string_ostream(s);
    v->print(*strm);
    std::string inst = strm->str();
    return "\"" + inst + "\"";
  }
}

namespace {
class FunctionInfo : public ModulePass {
public:
  static char ID;
  FunctionInfo() : ModulePass(ID) {}
  ~FunctionInfo() {}

  std::map<Value *, std::vector<Value *>> points_to;

  // Print output for each function
  bool runOnModule(Module &M) override {

    CallGraph CG = CallGraph(M);

    for (auto &F : M) {
      for (auto IT = df_begin(CG[&F]), EI = df_end(CG[&F]); IT != EI; IT++) {
        if (Function *F = IT->getFunction()) {
          outs() << "Visiting function: " << F->getName() << "\n";
        }
      }
        outs() << "------------------------\n";
    }
    /*
        if (F.getName().find("push_back") == std::string::npos)
          return false;

        outs() << F.getName() << "\n";

        for (auto &BB : F) {
          for (auto &I : BB) {
            if (GetElementPtrInst *IP = dyn_cast<GetElementPtrInst>(&I)) {

              points_to[IP].push_back(IP->getPointerOperand());
            }
            if (isa<PointerType>(I.getType())) {
            }
          }
        }*/
    return false;
  }
};
} // namespace

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("pointer", "15745: Opts", false, false);
