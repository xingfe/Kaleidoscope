#include "parser.hpp"
#include "lexer.hpp"
#include <stdio.h>
#include <iostream>
#include <map>
using namespace std;

static void HandleTopLevelExpression() {
    if (ParseDefinition()) {
        std::clog <<"Parsed a top-level expr " << endl;
        return;
    }
    getNextToken();
}

static void HandleExtern() {
    if (ParseExtern()) {
        std::clog << "Parsed an extern" <<endl;
        return;
    }
    getNextToken();
}

static void HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed an function definition\n");
        return ;
    }
    getNextToken();
}

/// top ::= definition | external | expression |';'


static std::map<char, int> binopPrecedence;
int main() {
    binopPrecedence['<'] = 10;
    binopPrecedence['+'] = 20;
    binopPrecedence['-'] = 20;
    binopPrecedence['*'] = 40;
    while(true) {
        fprintf(stderr, "ready> ");
        Token* token = getNextToken();
        std::clog << "currentToken:" << token->toString() <<endl;
        if(token->isEof()){
            return 0;
        }else if(token->isSemicolon()){
            getNextToken();
        }else if (token->isDef()){
            HandleDefinition();
        }else if (token->isExtern()){
            std::clog << "start handle extern" <<endl;
            HandleExtern();
        }else {
            HandleTopLevelExpression();
        }
    }
    return 0;
}


