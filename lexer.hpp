#pragma once 
#include <string>
#include <memory>
enum  EToken{
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,
};

struct Token{
    std::string toString(){
        throw std::logic_error{"Function not yet implemented."};
    }
    double getNumVal(){
        throw std::logic_error{"Function not yet implemented."};
    }
    bool isChar(int c){
        return false;
    }
    std::string getString(){
        throw std::logic_error{"Function not yet implemented."};
    }
    int getTokenPrecedence(){
        throw std::logic_error{"Function not yet implemented."};
    }
    int getChar(){
        throw std::logic_error{"Function not yet implemented."};
    }
    
    bool equals(const Token& token){
        return false;
    }

    bool isIdentifier(){
        return false;
    }
    bool isNumber(){
        return false;
    }

    bool isEof(){
        return false;
    }
    bool isSemicolon(){
        return false;
    }
    bool isDef(){
        return false;
    }
    bool isExtern(){
        return false;
    }
    virtual ~Token(){}
};

// static int getTokenPrecedence() {

//     if (!BinopOrecedeceIsInit) {
//         BinopOrecedece['<'] = 10; // lowest 
//         BinopOrecedece['+'] = 20;
//         BinopOrecedece['-'] = 30;
//         BinopOrecedece['*'] = 40; // highest
//     }
    
//     if (!isascii(currentToken)) {
//         return -1;
//     }

//     int TokenPrecedence = BinopOrecedece[currentToken];

//     if (TokenPrecedence <= 0) {
//         return -1;
//     }

//     return TokenPrecedence;
// }

struct TokenEOF :public Token{
    std::string toString() { return "EOF";}
};
struct TokenDef :public Token{
    
};
struct TokenExtern :public Token{
};
struct TokenFailed :public Token{
};
struct TokenIndentifier :public Token{
    std::string _value;
    TokenIndentifier(const std::string& value):_value(value){}
    std::string toString() { return "Indentifier"+ _value;}
};
struct TokenChar:public Token{
    int  _value;
    TokenChar(const int value):_value(value){}
};
struct TokenNumber :public Token{
    double _value;
    TokenNumber(double value):_value(value){}
};

template <typename TokenType>
std::shared_ptr<Token> buildToken(){
    static std::shared_ptr<Token> Token_EOF = std::make_shared<Token>();
    if(!Token_EOF->isEof()){
        Token_EOF.reset(new TokenType());
    }
    return Token_EOF;
}
std::shared_ptr<Token> buildToken(std::string name);

std::shared_ptr<Token> getToken();
    

