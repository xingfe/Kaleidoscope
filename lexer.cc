#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string>
#include "lexer.hpp"
using namespace std;
// lexer return tokens [0-255] if it is an unknown character,

int currentChar;

//
static shared_ptr<Token> getNoSpaceToken();
static shared_ptr<Token> getNumberToken();

static bool isDigit(int currentChar) {
    return isdigit(currentChar) || currentChar == '.';
}
shared_ptr<Token> getToken(){
    currentChar = getchar();
    while(isspace(currentChar)) {
        currentChar = getchar();
    }
    
    assert(!isspace(currentChar));
    if (currentChar == EOF) {
        return buildToken<TokenEOF>();
    }else if (isalnum(currentChar)) {
        return getNoSpaceToken();
    }else {
        return buildToken<TokenFailed>();
    }
}

static shared_ptr<Token> getIndentifierToken() {
    std::string identifierStr = "";
    
    for(char currentChar = getchar();isalnum(currentChar); currentChar= getchar() ){
        identifierStr += currentChar;
    }
    if (identifierStr == "def") {
        return buildToken<TokenDef>();
    }else if(identifierStr == "extern") {
        return buildToken<TokenExtern>();
    }
    return buildToken(identifierStr);
}

static shared_ptr<Token> getNoSpaceToken(){
    
    assert(!isspace(currentChar));
    if(isalpha(currentChar)){
        return getIndentifierToken();
    }else if(isDigit(currentChar)){
        return getNumberToken();
    }else if(currentChar == '#') {
        for(currentChar = getchar(); currentChar != '\n';
            currentChar = getchar()) {
        }
        return getToken();
    }
    //assert(false && "no space no support");
    return buildToken<TokenFailed>();
}

static shared_ptr<Token> getNumberToken() {
    assert(isDigit(currentChar));
    std::string numStr;
    numStr += currentChar;
    for(; isDigit(currentChar); currentChar = getchar()) {
        numStr += currentChar;
    }
    return std::make_shared<TokenNumber>(strtod(numStr.c_str(), 0));
}




 std::shared_ptr<Token> buildToken(std::string name){
    static std::shared_ptr<Token> Token_EOF = std::make_shared<Token>();
    if(!Token_EOF->isEof()){
        Token_EOF.reset(new TokenIndentifier(name));
    }
    return Token_EOF;
}
