#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

// These are multicharacter tokens and EOF. All other single character tokens
// are represented with their positive character code.
enum TokenType {
  TokenEOF = -1,
  TokenConst = -2,
  TokenElse = -3,
  TokenFalse = -4,
  TokenFunction = -5,
  TokenIdentifier = -6,
  TokenIf = -7,
  TokenLet = -8,
  TokenNumericLiteral = -9,
  TokenTrue = -10,
};

class Lexer {
  public:
    Lexer(const char* filename);
    int GetToken();
    int current;

  private:
    FILE* _stream = nullptr;
    // TODO(cjihrig): Track filename too.
    int _last = ' ';
    int _line = 0;
    int _col = 0;
};

Lexer::Lexer(const char* filename) {
  _stream = fopen(filename, "r");
}

int Lexer::GetToken() {
  string token;

  // Consume whitespace between tokens.
  while (isspace(_last)) {
    _last = fgetc(_stream);
  }

  // Identifiers and keywords all begin with a letter.
  if (isalpha(_last)) {
    token = _last;

    while (isalnum(_last = fgetc(_stream))) {
      token += _last;
    }

    if (token == "if") {
      current = TokenIf;
    } else if (token == "const") {
      current = TokenConst;
    } else if (token == "false") {
      current = TokenFalse;
    } else if (token == "let") {
      current = TokenLet;
    } else if (token == "true") {
      current = TokenTrue;
    } else {
      current = TokenIdentifier;
    }

    return current;
  }

  if (isdigit(_last) || _last == '.') {
    bool seen_point = false;

    do {
      seen_point = seen_point || _last == '.';
      token += _last;
      _last = fgetc(_stream);
    } while (isdigit(_last) || (seen_point == false && _last == '.'));

    // TODO(cjihrig): Support scientific notation.
    current = TokenNumericLiteral;
    return current;
  }

  current = _last;

  if (current != EOF) {
    _last = fgetc(_stream);
  }

  return current;
}

class Parser {
  public:
    Parser(Lexer* lexer);
    void Parse();

  private:
    bool Expect(int token, int expected);
    int ParseBlockStatement();
    int ParseEmptyStatement();
    int ParseExpression();
    int ParseFunctionDeclaration();
    int ParseIfStatement();
    int ParsePrimaryExpression();
    int ParseSourceElement();
    int ParseSourceElementList();
    int ParseStatement();
    int ParseStatementList();
    Lexer* _lexer;
};

Parser::Parser(Lexer* lexer) {
  _lexer = lexer;
}

void Parser::Parse() {
  _lexer->GetToken();
  ParseSourceElementList();
  Expect(_lexer->current, TokenEOF);
}

bool Parser::Expect(int token, int expected) {
  if (token != expected) {
    printf("SYNTAX ERROR: expected %c (%d) but got %c (%d)\n", expected, expected, token, token);
    return false;
  }

  return true;
}

int Parser::ParseBlockStatement() {
  printf("Started a block statement!\n");
  Expect(_lexer->current, '{');
  _lexer->GetToken();
  ParseStatementList();
  Expect(_lexer->current, '}');
  _lexer->GetToken();
  printf("Finished a block statement!\n");
  return 0;
}

int Parser::ParseEmptyStatement() {
  printf("Started an empty statement!\n");
  Expect(_lexer->current, ';');
  _lexer->GetToken();
  printf("Finished an empty statement!\n");
  return 0;
}

int Parser::ParseExpression() {
  printf("Started an expression!\n");
  ParsePrimaryExpression();
  printf("Finished an expression!\n");
  return 0;
}

int Parser::ParseFunctionDeclaration() {
  printf("Started a function declaration!\n");
  Expect(_lexer->current, TokenTrue);
  printf("Finished a function declaration!\n");
  return 0;
}

int Parser::ParseIfStatement() {
  printf("Started an if statement!\n");
  Expect(_lexer->current, TokenIf);
  Expect(_lexer->GetToken(), '(');
  _lexer->GetToken();
  ParseExpression();
  Expect(_lexer->current, ')');
  _lexer->GetToken();
  ParseBlockStatement();
  _lexer->GetToken();
  printf("Finished an if statement!\n");
  return 0;
}

int Parser::ParsePrimaryExpression() {
  printf("Started a primary expression\n");
  Expect(_lexer->current, TokenTrue);
  _lexer->GetToken();
  printf("Finished a primary expression!\n");
  return 0;
}

int Parser::ParseSourceElement() {
  printf("Started a source element\n");

  int r;

  if (_lexer->current == TokenFunction) {
    // Function declaration.
    r = ParseFunctionDeclaration();
  } else {
    // Statement.
    r = ParseStatement();
  }

  printf("Finished a source element\n");
  return r;
}

int Parser::ParseSourceElementList() {
  printf("Started a source element list\n");
  while (ParseSourceElement() == 0);
  printf("Finished a source element list\n");
  return 0;
}

int Parser::ParseStatement() {
  printf("Started a statement\n");
  int token = _lexer->current;

  if (token == TokenIf) {
    // If statement.
    ParseIfStatement();
  } else if (token == '{') {
    // Block statement.
    ParseBlockStatement();
  } else if (token == ';') {
    // Empty statement.
    ParseEmptyStatement();
  } else {
    return -1;
  }

  printf("Finished a statement\n");
  return 0;
}

int Parser::ParseStatementList() {
  printf("Started a statement list\n");
  while (ParseStatement() == 0);
  printf("Finished a statement list\n");
  return 0;
}

int main(void) {
  Lexer* lexer = new Lexer("if-with-statement.x");
  // Lexer* lexer = new Lexer("if.x");
  // Lexer* lexer = new Lexer("foo.x");
  // Lexer* lexer = new Lexer("empty.x");
  // Lexer* lexer = new Lexer("two-semicolons.x");
  // Lexer* lexer = new Lexer("semicolon.x");
  Parser* parser = new Parser(lexer);

  parser->Parse();

  // ifstream input_file;

  // input_file.open("foo.x");

  // if (!input_file.is_open()) {
  //   cout << "Could not open file!" << endl;
  //   return -1;
  // }

  // cout << "Opened file!" << endl;
  // input_file.close();

  return 0;
}
