#include <string>
#include <vector>
#include <llvm/IR/LLVMContext.h>
class ExprAST{
public:
    virtual ~ExprAST(){}
    virtual Value *codegen() = 0;
};

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;

Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

class NumberExprAST : public ExprAST {
    double Val;
public:
    NumberExprAST(double Val) : Val(Val){}
    Value *NumberExprAST::codegen() {
        return ConstantFP::get(TheContext, APFloat(Val));
    }
};

class VariableExprAST : public ExprAST {
    std::string Name;
public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
    Value *VariableExprAST::codegen() {
  // Look this variable up in the function.
  Value *V = NamedValues[Name];
  if (!V)
    LogErrorV("Unknown variable name");
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
    Value *BinaryExprAST::codegen(){
        Value* left = left->codegen();
        Value* right = right->codegen();
        if(left == NULL || right == NULL){
            return nullptr;
        }
        switch(op){
        case '+':
            return Builder.CreateFAdd(left, right, "addtmp");
        case '-':
            return Builder.CreateFSub(left, right, "subtmp");
        case '*':
            return Builder.CreateFMul(left, right, "multmp");
        case '<':
            left = Builder.CreateFCmpULT(left, right, "cmptmp");
            return Builder.CreateUIToFP(left, Type::getDoubleTy(TheContext), "booltmp");
        default:
            return LogErrorV("invalid binary operator");
        }
    }
};


class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
public:
    PrototypeAST(const std::string &name, std::vector<std::string> Args)
            :Name(name), Args(std::move(Args)){}
    inline const std::string & getName() const {return Name;}
};





class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
public:
    CallExprAST(const std::string& Callee,
                std::vector<std::unique_ptr<ExprAST>> Args) 
        :Callee(Callee), Args(std::move(Args)){}
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
            :Proto(std::move(Proto)), Body(std::move(Body)) {}
};



