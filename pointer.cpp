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
#include <string.h>
#include <unordered_map>
#include <unordered_set>

using namespace llvm;

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
std::map<Function *, int> stats;

typedef struct type {
  bool isLoc;
  Value *v;
  std::string call_string;

  bool operator==(const type &t) const { return (this->v == t.v); }

} type_t;

class MyHashFunction {
public:
  size_t operator()(const type &t) const { return (size_t)t.v; }
};

inline bool operator<(const type_t &lhs, const type_t &rhs) {
  return lhs.v < rhs.v;
}

class FunctionInfo : public ModulePass {
public:
  static char ID;
  FunctionInfo() : ModulePass(ID) {}
  ~FunctionInfo() {}

  int notAlias = 0;
  int mustAlias = 0;
  int mayAlias = 0;

  std::unordered_map<type_t, std::unordered_set<type_t, MyHashFunction>,
                     MyHashFunction>
      points_to;

  void evaluate(Function *F, int depth, std::string call_string) {
    if (depth > 5)
      return;

    std::set<Value *> ptrs;

    for (auto &I : F->args())
      if (isInterestingPointer(&I)) // Add all pointer arguments.
        ptrs.insert(&I);

    for (auto &BB : *F) {
      for (auto &I : BB) {
        if (isInterestingPointer(&I)) {
          ptrs.insert(&I);
        }
      }
    }

    for (auto *ptr1 : ptrs) {
      for (auto *ptr2 : ptrs) {

        if (points_to[type_t{false, ptr1, call_string}].size() == 0 ||
            points_to[type_t{false, ptr2, call_string}].size() == 0) {

          continue;
        }
        if (ptr1 == ptr2)
          continue;

        int intersection = 0;

        for (auto &pointees1 : points_to[type_t{false, ptr1, call_string}]) {
          for (auto &pointees2 : points_to[type_t{false, ptr2, call_string}]) {
            if (pointees1.v == pointees2.v) {
              intersection++;
              break;
            }
          }
        }

        if (intersection == 0) {
          notAlias++;
        } else if (intersection ==
                       points_to[type_t{false, ptr1, call_string}].size() &&
                   intersection ==
                       points_to[type_t{false, ptr2, call_string}].size()) {
          mustAlias++;
        } else {
          mayAlias++;
        }
      }
    }

    for (auto &B : *F) {
      for (auto &I : B) {
        if (auto *CI = dyn_cast<CallInst>(&I)) {
          if (auto *Func = CI->getCalledFunction()) {
            evaluate(Func, depth + 1, call_string + Func->getName().str());
          }
        }
      }
    }
  }

  void andersen(Function *F, int depth, std::string call_string) {
    if (depth > 5 || !F) {

      return;
    }

    for (auto &BB : *F) {
      for (auto &I : BB) {
        if (auto *SI = dyn_cast<StoreInst>(&I)) {

          points_to[type_t{false, SI->getPointerOperand(), call_string}].insert(
              type_t{isa<AllocaInst>(SI->getValueOperand()),
                     SI->getValueOperand(), call_string});

        } else if (auto *LI = dyn_cast<LoadInst>(&I)) {

          for (auto &pointee :
               points_to[type_t{false, LI->getPointerOperand(), call_string}]) {

            for (auto &pointe2 :
                 points_to[type_t{false, pointee.v, call_string}]) {
              points_to[type_t{false, LI, call_string}].insert(pointe2);
            }
          }

        } else if (auto *CI = dyn_cast<CallInst>(&I)) {

           if (!CI->getCalledFunction())
             continue;

           auto *dop = CI->data_operands_begin();
           for (auto &op : CI->getCalledFunction()->args()) {

             if (isa<PointerType>(op.getType())) {
               for (auto &pointee : points_to[type_t{false, dop->get(), call_string}]) {
                 points_to[type_t{false, &op, call_string}].insert(pointee);
               }
             }

             dop++;
           }

           andersen(CI->getCalledFunction(), depth + 1, call_string +
         CI->getCalledFunction()->getName().str());

           for (auto &BBB : *CI->getCalledFunction()) {
             for (auto &II : BBB) {
               if (auto *ret = dyn_cast<ReturnInst>(&II)) {
                 if (ret->getReturnValue() &&
                     ret->getReturnValue()->getType()->isPointerTy()) {
                   for (auto &pointee : points_to[type_t{false, ret->getReturnValue(), call_string}]) {
                     points_to[type_t{false, CI, call_string}].insert(pointee);
                   }

                   // points_to[CI].insert(ret->getReturnValue());
                 }
               }
             }
           }

         }
        else if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {

          for (auto &pointee : points_to[type_t{false, GEP->getPointerOperand(),
                                                call_string}]) {
            points_to[type_t{false, GEP, call_string}].insert(pointee);
          }

        } else if (auto *PHI = dyn_cast<PHINode>(&I)) {
          for (auto &I : PHI->incoming_values()) {
            for (auto &pointee : points_to[type_t{false, I, call_string}]) {
              points_to[type_t{false, PHI, call_string}].insert(pointee);
            }
          }
        } else if (auto *BIT = dyn_cast<BitCastInst>(&I)) {
          for (auto op : BIT->operand_values()) {
            for (auto &pointee : points_to[type_t{false, op, call_string}]) {
              points_to[type_t{false, BIT, call_string}].insert(pointee);
            }
          }

        } else if (auto *INT = dyn_cast<IntToPtrInst>(&I)) {
          for (auto op : INT->operand_values()) {
            for (auto &pointee : points_to[type_t{false, op, call_string}]) {
              points_to[type_t{false, INT, call_string}].insert(pointee);
            }
          }
        } else if (auto *SEL = dyn_cast<SelectInst>(&I)) {
          for (auto &op : SEL->operands()) {
            for (auto &pointee : points_to[type_t{false, op, call_string}]) {

              points_to[type_t{false, SEL, call_string}].insert(pointee);
            }
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

    /*
        for (auto &G : M.globals()) {
          if (G.getType()->isPointerTy() && isa<AllocaInst>(G)) {
            points_to[&G].insert(type_t{isa<AllocaInst>(&G), &G});
          }
        }

        for (auto &F : M) {
        }*/

    for (auto &F : M) {
      for (auto &B : F) {
        for (auto &I : B) {
          if (I.getType()->isPointerTy()) {
            points_to[type_t{false, &I, F.getName().str()}].insert(
                type_t{true, &I});
          }
        }
      }

      andersen(&F, 0, F.getName().str());
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

    for (auto &F : M) {

      evaluate(&F, 0, F.getName().str());
    }

    int tote = notAlias + mustAlias + mayAlias;
    printf("tote: %d\n", tote);
    printf("Not: %f\n", (float)notAlias / tote);
    printf("May: %f\n", (float)mayAlias / tote);
    printf("Must: %f\n", (float)mustAlias / tote);

    return false;

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

          if (points_to[type_t{false, ptr1, F.getName().str()}].size() == 0 ||
              points_to[type_t{false, ptr2, F.getName().str()}].size() == 0) {

            continue;
          }
          if (ptr1 == ptr2)
            continue;

          int intersection = 0;

          for (auto &pointees1 :
               points_to[type_t{false, ptr1, F.getName().str()}]) {
            for (auto &pointees2 :
                 points_to[type_t{false, ptr2, F.getName().str()}]) {
              if (pointees1.v == pointees2.v) {
                intersection++;
                break;
              }
            }
          }

          if (intersection == 0) {
            notAlias++;
          } else if (intersection ==
                         points_to[type_t{false, ptr1, F.getName().str()}]
                             .size() &&
                     intersection ==
                         points_to[type_t{false, ptr2, F.getName().str()}]
                             .size()) {
            mustAlias++;
          } else {
            mayAlias++;
          }
        }
      }
    }

    printf("tote: %d\n", tote);
    printf("Not: %f\n", (float)notAlias / tote);
    printf("May: %f\n", (float)mayAlias / tote);
    printf("Must: %f\n", (float)mustAlias / tote);

    return true;

    // printf("Total: %d\n", total);
    // printf("Must: %f\n", (float)must / (float)total);
    // printf("May: %f\n", 1 - (float)must / (float)total);

    return true;
  }
};
} // namespace

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("pointer", "15745: Opts", false, false);
