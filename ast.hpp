#include <string>
#include <vector>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

class ExprAST{
public:
    virtual ~ExprAST(){}
    virtual llvm::Value *codegen() = 0;
};
static llvm::LLVMContext TheContext;
static std::unique_ptr<llvm::Module> TheModule;
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
public:
    PrototypeAST(const std::string &name, std::vector<std::string> Args)
            :Name(name), Args(std::move(Args)){}
    inline const std::string & getName() const {return Name;}
    llvm::Function* codegen(){
        std::vector<llvm::Type *> Doubles(Args.size(), llvm::Type::getDoubleTy(TheContext));
        llvm::FunctionType *FT =
                llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext), Doubles, false);
        llvm::Function *F =
                llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule.get());

        // Set names for all arguments.
        unsigned Idx = 0;
        for (auto &Arg : F->args()){
            Arg.setName(Args[Idx++]);
        }
        
        return F;
        
    }
        
};



static llvm::IRBuilder<> Builder(TheContext);

static std::map<std::string, llvm::Value *> NamedValues;
static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

llvm::Function *getFunction(std::string Name) {
    if (auto *F = TheModule->getFunction(Name)){
        return F;
    }
    // if not , checker
    auto FI = FunctionProtos.find(Name);
    if (FI != FunctionProtos.end()){
        return FI->second->codegen();
    }
    return nullptr;
        
}


std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

class NumberExprAST : public ExprAST {
    double Val;
public:
    NumberExprAST(double Val) : Val(Val){}
    llvm::Value *codegen() {
        return llvm::ConstantFP::get(TheContext, llvm::APFloat(Val));
    }
};

class VariableExprAST : public ExprAST {
    std::string Name;
public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
    llvm::Value *codegen() {
  // Look this variable up in the function.
        llvm::Value *V = NamedValues[Name];
        if (!V){
            return LogErrorV("Unknown variable name");
        }
        return V;
    }
};



class BinaryExprAST : public ExprAST {
    char op;
    std::unique_ptr<ExprAST> left, right;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> left,
                  std::unique_ptr<ExprAST> right):
            op(op), left(std::move(left)), right(std::move(right)){}
    llvm::Value *codegen() override{
        llvm::Value* leftValue = left->codegen();
        llvm::Value* rightValue = right->codegen();
        if(left == NULL || right == NULL){
            return nullptr;
        }
        switch(op){
        case '+':
            return Builder.CreateFAdd(leftValue, rightValue, "addtmp");
        case '-':
            return Builder.CreateFSub(leftValue, rightValue, "subtmp");
        case '*':
            return Builder.CreateFMul(leftValue, rightValue, "multmp");
        case '<':
            leftValue = Builder.CreateFCmpULT(leftValue, rightValue, "cmptmp");
            return Builder.CreateUIToFP(leftValue,
                                        llvm::Type::getDoubleTy(TheContext), "booltmp");
        default:
            return LogErrorV("invalid binary operator");
        }
    }
};







class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
public:
    CallExprAST(const std::string& Callee,
                std::vector<std::unique_ptr<ExprAST>> Args) 
        :Callee(Callee), Args(std::move(Args)){}
    llvm::Value* codegen(){
        llvm::Function *CalleeF = getFunction(Callee);
        if (!CalleeF){
            return LogErrorV("Unknown function referenced");
        }
        if(CalleeF->arg_size() != Args.size()){
            return LogErrorV("Incorrect # arguments passed");
        }
        std::vector<llvm::Value *> ArgsV;
        for (unsigned i = 0, e = Args.size(); i != e; ++i) {
            ArgsV.push_back(Args[i]->codegen());
            if (!ArgsV.back()){
                return nullptr;
            }
        }
        return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
    }
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
            :Proto(std::move(Proto)), Body(std::move(Body)) {}
};



