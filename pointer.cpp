#include "llvm/Analysis/CallGraph.h"
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DepthFirstIterator.h>
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

using namespace llvm;

std::map<Function *, int> stats;

bool sortByVal(const std::pair<Function *, int> &a,
               const std::pair<Function *, int> &b) {
  return (a.second < b.second);
}

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

  std::unordered_map<Value *, std::set<Value *>> points_to;

  void andersen(Function *F, int depth) {
    if (depth > 4)
      return;

    for (auto &BB : *F) {
      for (auto &I : BB) {
        if (auto *SI = dyn_cast<StoreInst>(&I)) {

          points_to[SI->getPointerOperand()].insert(SI->getValueOperand());

        } else if (auto *LI = dyn_cast<LoadInst>(&I)) {
          points_to[LI->getPointerOperand()].insert(LI);
        } else if (auto *CI = dyn_cast<CallInst>(&I)) {

          if (!CI->getCalledFunction())
            continue;

          andersen(CI->getCalledFunction(), depth + 1);

          auto *dop = CI->data_operands_begin();
          for (auto &op : CI->getCalledFunction()->args()) {

            if (isa<PointerType>(op.getType())) {
              for (auto *pointee : points_to[&op]) {
                points_to[dop->get()].insert(pointee);
              }
            }

            dop++;
          }

          for (auto &BBB : *CI->getCalledFunction()) {
            for (auto &II : BBB) {
              if (auto *ret = dyn_cast<ReturnInst>(&II)) {
                if (ret->getReturnValue()) {
                  points_to[CI].insert(ret->getReturnValue());
                }
              }
            }
          }

        } else if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
          points_to[GEP].insert(GEP->getPointerOperand());
        } else if (auto *PHI = dyn_cast<PHINode>(&I)) {
          for (auto &I : PHI->incoming_values()) {
            points_to[PHI].insert(I.get());
          }
        } else if (auto *BIT = dyn_cast<BitCastOperator>(&I)) {

          for (auto &op : BIT->operands()) {
            if (isa<PointerType>(op.get()->getType()))
              points_to[BIT].insert(op.get());
          }

        } else if (auto *INT = dyn_cast<IntToPtrInst>(&I)) {
          for (auto &op : INT->operands()) {
            points_to[INT].insert(op.get());
          }
        } else if (auto *SEL = dyn_cast<SelectInst>(&I)) {
          for (auto &op : SEL->operands()) {
            points_to[SEL].insert(op.get());
          }
        }
      }
    }
  }

  static inline bool isInterestingPointer(Value *V) {
    return V->getType()->isPointerTy() && !isa<ConstantPointerNull>(V);
  }

  // Print output for each function
  bool runOnModule(Module &M) override {

    CallGraph CG = CallGraph(M);
    Function *f = M.getFunction("main");

    for (auto &F : M) {
      andersen(&F, 0);
    }

    int must = 0;
    int total = 0;
    int total_ptrs = 0;

    for (auto &F : M) {

      for (auto &BB : F) {
        for (auto &I : BB) {
          if (isa<PointerType>(I.getType())) {
            total_ptrs++;
          }
        }
      }
    }

    printf("total ptrs: %d\n", total_ptrs);

    int notAlias = 0;
    int mustAlias = 0;
    int mayAlias = 0;

    for (auto &F : M) {
      std::set<Value *> ptrs;

      for (auto &I : F.args())
        if (isInterestingPointer(&I)) // Add all pointer arguments.
          ptrs.insert(&I);

      for (auto &BB : F) {
        for (auto &I : BB) {
          if (isInterestingPointer(&I)) {
            ptrs.insert(&I);
          }
        }
      }

      for (auto *ptr1 : ptrs) {
        for (auto *ptr2 : ptrs) {
          if (ptr1 == ptr2)
            continue;

          int intersection = 0;

          for (auto *pointees1 : points_to[ptr1]) {
            for (auto *pointees2 : points_to[ptr2]) {
              if (pointees1 == pointees2) {
                intersection++;
                break;
              }
            }
          }

          if (intersection == points_to[ptr1].size() &&
              intersection == points_to[ptr2].size()) {
            mustAlias++;
          } else if (intersection == 0) {
            notAlias++;
          } else {
            mayAlias++;
          }
        }
      }
    }

    int tote = notAlias + mustAlias + mayAlias;
    printf("tote: %d\n", tote);
    printf("Not: %f\n", (float)notAlias / tote);
    printf("May: %f\n", (float)mayAlias / tote);
    printf("Must: %f\n", (float)mustAlias / tote);


    for (auto &pair : points_to) {
      int count = 0;
      if (!pair.first->getType()->isPointerTy())
        continue;
      for (auto *pointee : pair.second) {

        if (!pointee->getType()->isPointerTy())
          continue;

        count++;
        auto *ptr = dyn_cast<Instruction>(pair.first);
        auto *ptee = dyn_cast<Instruction>(pointee);
        auto *argtee = dyn_cast<Argument>(pointee);

        continue;

        if (ptr && ptee) {
          outs() << getShortValueName(ptr) << "("
                 << ptr->getFunction()->getName() << ")"
                 << " -> " << getShortValueName(ptee) << "("
                 << ptee->getFunction()->getName() << ")"
                 << "\n";
        } else if (ptr && argtee) {
          outs() << getShortValueName(ptr) << "("
                 << ptr->getFunction()->getName() << ")"
                 << " -> " << getShortValueName(argtee) << "("
                 << argtee->getParent()->getName() << ")"
                 << "\n";

        } else {
          outs() << getShortValueName(pair.first) << "("
                 << pair.first->getName() << ")"
                 << " -> " << getShortValueName(pointee) << "\n";
        }
      }

      if (count == 1) {
        must++;
      }
      total++;
    }


    return true;

    printf("Total: %d\n", total);
    printf("Must: %f\n", (float)must / (float)total);
    printf("May: %f\n", 1 - (float)must / (float)total);

    return true;
  }
};
} // namespace

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("pointer", "15745: Opts", false, false);
