#include "lexer.hpp"
#include "ast.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <glog/logging.h>

using namespace std;
using namespace google;
std::shared_ptr<Token> currentToken;

static std::unique_ptr<ExprAST> ParseExperssion() ;

static std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto result = std::make_unique<NumberExprAST>(currentToken->getNumVal());
    return std::move(result);
}
std::shared_ptr<Token> getNextToken(){
    currentToken = getToken();
    return currentToken;
}
static std::unique_ptr<ExprAST> ParseParenExpr() {
    getNextToken(); //eat (
    auto v = ParseExperssion();
    if (!v) {
        return nullptr;
    }
    if (!currentToken->isChar(')')) {
        std::clog <<"exepected ( but given" << currentToken;
        return nullptr;
    }
    getNextToken(); // eat )
    return v;
}

// return the precedence for the current token , or -1 

static std::unique_ptr<ExprAST> ParseIndentifierExpr() {
    std::string idName = currentToken->getString();
    getNextToken(); // eat identifierstr;
    if (!currentToken->isChar('(') ){
        return std::make_unique<VariableExprAST>(idName);
    }

    getNextToken(); // eat (

    std::vector<std::unique_ptr<ExprAST>> Args;
    
    if (!currentToken->isChar(')')) {
        bool done = false;
        while(!done) {
            auto Arg = ParseExperssion();
            if (Arg) {
                Args.push_back(std::move(Arg));
            }
            
            if (currentToken->isChar(')')){
                done = true;
                continue;
            }
            if (currentToken->isChar(',') ){
                getNextToken();
                continue;
            }
            
            std::clog << "Expected ') or ',', in argument list";
            return nullptr;
        }
    }

    
    getNextToken(); // eat );

    return std::make_unique<CallExprAST>(idName, std::move(Args));
}

// primary
//  := identifierexpr
//  := numberexpr
//  := parenexpr
static std::unique_ptr<ExprAST> ParsePrimary() {
    if(currentToken->isIdentifier()){
        return ParseIndentifierExpr();
    }else if(currentToken->isNumber()){
        return ParseNumberExpr();
    }else if(currentToken->isChar('(')){
        return ParseParenExpr();
    }else {
        std::cerr << "unknown token when expecting an expression "<< ":"<<
                currentToken;
        return nullptr;
    }
}

// expression
//  :: = primary (op primary)*
static std::unique_ptr<ExprAST>
ParseBinOpRight(int exprPrec,
                std::unique_ptr<ExprAST> left) ;

static std::unique_ptr<ExprAST> ParseExperssion() {
    auto left = ParsePrimary();

    if (!left) {
        return nullptr;
    }

    return ParseBinOpRight(0, std::move(left));
}

// precedence value passed into ParseBinopRight indicates the minimal operator
// precedence that function is allowd to eat. 
// 
static std::unique_ptr<ExprAST>
ParseBinOpRight(int exprPrec,
                std::unique_ptr<ExprAST> left) {
    //  
    int tokPrec = currentToken->getTokenPrecedence();
    // binoprh allowed to be empty, in which case it returns the expression that
    // is passed into it.
    if (tokPrec < exprPrec) {
        return left;
    }
    
    int op = currentToken->getChar();
    getNextToken();
    
    auto right = ParsePrimary();
    if (!right) {
        return nullptr;
    }
    
    int nextPrec = currentToken->getTokenPrecedence() ;
    if (tokPrec < nextPrec){
        right = ParseBinOpRight(tokPrec + 1, std::move(right));
        if (!right) {
            return nullptr;
        }
    }
    
    return ParseBinOpRight(tokPrec,
                           std::make_unique<BinaryExprAST>(op, std::move(left),
                                                           std::move(right)));

}

// prototype
// := id '(' id * ')'
static std::unique_ptr<PrototypeAST> ParsePrototype() {
    std::clog << "in parse prototype" ;
    if (!currentToken->isIdentifier()){
        std::clog << "Expected function name in prototype";
        return nullptr;
    }
    
    std::string functionName = currentToken->getString();
    getNextToken();

    if (!currentToken->isChar('(')) {
        std::clog << "expected '(' in prototype";
        return nullptr;
    }

    vector<string> ArgsNames;

    while(true) {
        std::shared_ptr<Token> token = getNextToken();
        if (!token->isIdentifier()) {
            break;
        }
        ArgsNames.push_back(token->getString());

    }

    if (!currentToken->isChar(')')) {
        std::clog << "expected ')' in prototype";
        return nullptr;
    }
    getNextToken(); // eat ')'

    std::clog << "end parse prototype "<< functionName ;
    for(size_t i = 0 ; i< ArgsNames.size() ; i++) {
        std::clog << ";"<< ArgsNames[i] ;
    }
    std::clog <<endl;
    // return std::make_unique<PrototypeAST>("xx", vector<string>());
    return std::make_unique<PrototypeAST>(functionName, std::move(ArgsNames));
}

// defintion := 'def' prototype expression

std::unique_ptr<FunctionAST> ParseDefinition() {
    getNextToken() ;  // eat def.
    auto Proto = ParsePrototype();
    if (!Proto) {
        return nullptr;
    }

    if (auto E = ParseExperssion()) {
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }

    return nullptr;
    
}

// exteral := 'extern' prototype

std::unique_ptr<PrototypeAST> ParseExtern() {
    std::clog << "in parse extern" <<endl;
    getNextToken();  // eat extern;
    return ParsePrototype();
}

// toplevelexpr := expression

static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E = ParseExperssion()) {
        auto Proto = std::make_unique<PrototypeAST>("", vector<string>());
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}


