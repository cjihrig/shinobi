#include "ast.h"
#include "lexer.h"
#include "parser.h"

using namespace std;
using namespace llvm;

Lexer::Lexer(string filename) {
  _stream = nullptr;
  _filename = filename;
  _lexeme = "";
  _token = TokenInvalid;
  _last = ' ';  // Fake previous token to start the lexer.
  _line = 0;
  _start_col = 0;
  _col = -1;
}

Lexer::~Lexer() {
  if (_stream != nullptr) {
    fclose(_stream);
  }
}

int Lexer::Consume() {
  _last = fgetc(_stream);

  return _last;
}

int Lexer::Column() {
  return _start_col;
}

string Lexer::Lexeme() {
  return _lexeme;
}

int Lexer::Line() {
  return _line;
}

int Lexer::NextToken() {
  // Consume whitespace between tokens.
  while (isspace(_last)) {
    if (_last == '\n') {
      _line++;
      _col = 0;
    } else {
      _col++;
    }

    Consume();
  }

  _start_col = _col;

  // Identifiers and keywords all begin with a letter.
  if (isalpha(_last)) {
    _lexeme = _last;

    while (isalnum(Consume())) {
      _lexeme += _last;
    }

    if (_lexeme == "if") {
      _token = TokenIf;
    } else if (_lexeme == "const") {
      _token = TokenConst;
    } else if (_lexeme == "function") {
      _token = TokenFunction;
    } else if (_lexeme == "false") {
      _token = TokenFalse;
    } else if (_lexeme == "let") {
      _token = TokenLet;
    } else if (_lexeme == "true") {
      _token = TokenTrue;
    } else if (_lexeme == "return") {
      _token = TokenReturn;
    } else {
      _token = TokenIdentifier;
    }

    _col += _lexeme.size();
    return _token;
  }

  _lexeme = "";

  if (isdigit(_last) || _last == '.') {
    bool seen_point = false;

    do {
      seen_point = seen_point || _last == '.';
      _lexeme += _last;
      Consume();
    } while (isdigit(_last) || (seen_point == false && _last == '.'));

    // TODO(cjihrig): Support scientific notation.
    _token = TokenNumericLiteral;
    _col += _lexeme.size();
    return _token;
  }

  if (_last == '\'') {
    // TODO(cjihrig): Need to handle escape sequences.
    Consume();

    while (_last != '\'') {
      _lexeme += _last;
      Consume();
      // TODO(cjihrig): Need to handle EOF, line breaks, etc.
    }

    // Consume the closing quote.
    if (_last == '\'') {
      Consume();
    }

    _token = TokenStringLiteral;
    _col += _lexeme.size();
    return _token;
  }

  if (_last == '/') {
    int next = Consume();

    if (next == '*') {
      // Multi-line comment.
    } else if (next == '/') {
      // Single line comment. Consume the comment and return the next token.
      while (Consume() != '\n');
      return NextToken();
    } else {
      // Division operator.
      _token = TokenDivide;
      _col++;
    }
  }

  if (_last == '&') {
    if (Consume() != '&') {
      // Bitwise AND operator.
      _token = TokenBitwiseAnd;
      _col++;
    } else {
      // Logical AND operator.
      _token = TokenLogicalAnd;
      _col += 2;
      Consume();
    }

    return _token;
  }

  if (_last == ';' || _last == '=' ||
      _last == '(' || _last == ')' ||
      _last == '{' || _last == '}' ||
      _last == ':' || _last == EOF) {
    _token = _last;
  } else {
    _token = TokenInvalid;
  }

  if (_token != EOF) {
    _col++;
    Consume();
  }

  return _token;
}

int Lexer::Open() {
  // TODO(cjihrig): Check that the stream is not already opened.
  _stream = fopen(_filename.c_str(), "r");

  if (_stream == nullptr) {
    llvm::errs() << "Could not open file '" << _filename << "': " <<
      strerror(errno) << "\n";
    return errno;
  }

  return 0;
}

int Lexer::Token() {
  return _token;
}
