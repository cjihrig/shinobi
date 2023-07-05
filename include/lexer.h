#ifndef __SHINOBI_LEXER_H__
#define __SHINOBI_LEXER_H__

#include <stdio.h>
#include <string>

// These are multicharacter tokens and EOF. All other single character tokens
// are represented with their positive character code.
enum TokenType {
  TokenInvalid = 0,
  TokenEOF = -1,
  TokenAdd = -2,
  TokenBitwiseAnd = -3,
  TokenBitwiseNot = -4,
  TokenBitwiseOr = -5,
  TokenConst = -6,
  TokenDivide = -7,
  TokenElse = -8,
  TokenFalse = -9,
  TokenFunction = -10,
  TokenIdentifier = -11,
  TokenIf = -12,
  TokenLet = -13,
  TokenLogicalAnd = -14,
  TokenLogicalNot = -15,
  TokenLogicalOr = -16,
  TokenMultiply = -17,
  TokenNumericLiteral = -18,
  TokenReturn = -19,
  TokenStringLiteral = -20,
  TokenSubtract = -21,
  TokenTrue = -22,
  TokenWhile = -23
};

class Lexer {
  public:
    Lexer(std::string filename);
    ~Lexer();
    int Column();
    std::string Lexeme();
    int Line();
    int NextToken();
    int Open();
    int Token();

  private:
    int Consume();

    FILE* _stream;
    std::string _filename;
    std::string _lexeme;
    int _token;
    int _last;
    int _line;
    int _start_col;
    int _col;
};

#endif /* __SHINOBI_LEXER_H__ */
