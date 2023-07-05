#include <assert.h>
#include "lexer.h"

int main(void) {
  Lexer* lexer = new Lexer("fixtures/tokens.x");

  assert(lexer->Open() == 0);

  // Verify initial state.
  assert(lexer->Token() == TokenInvalid);
  assert(lexer->Line() == 0);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");

  // Verify the contents of a file. Token by token.
  // {
  assert(lexer->NextToken() == '{');
  assert(lexer->Token() == '{');
  assert(lexer->Line() == 0);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");
  //  const foo = 1;
  assert(lexer->NextToken() == TokenConst);
  assert(lexer->Token() == TokenConst);
  assert(lexer->Line() == 1);
  assert(lexer->Column() == 2);
  assert(lexer->Lexeme() == "const");
  assert(lexer->NextToken() == TokenIdentifier);
  assert(lexer->Token() == TokenIdentifier);
  assert(lexer->Line() == 1);
  assert(lexer->Column() == 8);
  assert(lexer->Lexeme() == "foo");
  assert(lexer->NextToken() == '=');
  assert(lexer->Token() == '=');
  assert(lexer->Line() == 1);
  assert(lexer->Column() == 12);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == TokenNumericLiteral);
  assert(lexer->Token() == TokenNumericLiteral);
  assert(lexer->Line() == 1);
  assert(lexer->Column() == 14);
  assert(lexer->Lexeme() == "1");
  assert(lexer->NextToken() == ';');
  assert(lexer->Token() == ';');
  assert(lexer->Line() == 1);
  assert(lexer->Column() == 15);
  assert(lexer->Lexeme() == "");
  //  let bar = true;
  assert(lexer->NextToken() == TokenLet);
  assert(lexer->Token() == TokenLet);
  assert(lexer->Line() == 2);
  assert(lexer->Column() == 2);
  assert(lexer->Lexeme() == "let");
  assert(lexer->NextToken() == TokenIdentifier);
  assert(lexer->Token() == TokenIdentifier);
  assert(lexer->Line() == 2);
  assert(lexer->Column() == 6);
  assert(lexer->Lexeme() == "bar");
  assert(lexer->NextToken() == '=');
  assert(lexer->Token() == '=');
  assert(lexer->Line() == 2);
  assert(lexer->Column() == 10);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == TokenTrue);
  assert(lexer->Token() == TokenTrue);
  assert(lexer->Line() == 2);
  assert(lexer->Column() == 12);
  assert(lexer->Lexeme() == "true");
  assert(lexer->NextToken() == ';');
  assert(lexer->Token() == ';');
  assert(lexer->Line() == 2);
  assert(lexer->Column() == 16);
  assert(lexer->Lexeme() == "");
  // }
  assert(lexer->NextToken() == '}');
  assert(lexer->Token() == '}');
  assert(lexer->Line() == 3);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");
  // {
  assert(lexer->NextToken() == '{');
  assert(lexer->Token() == '{');
  assert(lexer->Line() == 5);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");
  //  const pi = 3.14159;
  assert(lexer->NextToken() == TokenConst);
  assert(lexer->Token() == TokenConst);
  assert(lexer->Line() == 6);
  assert(lexer->Column() == 2);
  assert(lexer->Lexeme() == "const");
  assert(lexer->NextToken() == TokenIdentifier);
  assert(lexer->Token() == TokenIdentifier);
  assert(lexer->Line() == 6);
  assert(lexer->Column() == 8);
  assert(lexer->Lexeme() == "pi");
  assert(lexer->NextToken() == '=');
  assert(lexer->Token() == '=');
  assert(lexer->Line() == 6);
  assert(lexer->Column() == 13);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == TokenNumericLiteral);
  assert(lexer->Token() == TokenNumericLiteral);
  assert(lexer->Line() == 6);
  assert(lexer->Column() == 17);
  assert(lexer->Lexeme() == "3.14159");
  assert(lexer->NextToken() == ';');
  assert(lexer->Token() == ';');
  assert(lexer->Line() == 6);
  assert(lexer->Column() == 25);
  assert(lexer->Lexeme() == "");
  //  if (foo && bar) {
  assert(lexer->NextToken() == TokenIf);
  assert(lexer->Token() == TokenIf);
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 2);
  assert(lexer->Lexeme() == "if");
  assert(lexer->NextToken() == '(');
  assert(lexer->Token() == '(');
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 5);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == TokenIdentifier);
  assert(lexer->Token() == TokenIdentifier);
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 6);
  assert(lexer->Lexeme() == "foo");
  assert(lexer->NextToken() == TokenLogicalAnd);
  assert(lexer->Token() == TokenLogicalAnd);
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 10);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == TokenIdentifier);
  assert(lexer->Token() == TokenIdentifier);
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 13);
  assert(lexer->Lexeme() == "bar");
  assert(lexer->NextToken() == ')');
  assert(lexer->Token() == ')');
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 16);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == '{');
  assert(lexer->Token() == '{');
  assert(lexer->Line() == 8);
  assert(lexer->Column() == 18);
  assert(lexer->Lexeme() == "");
  //    ;
  assert(lexer->NextToken() == ';');
  assert(lexer->Token() == ';');
  assert(lexer->Line() == 10);
  assert(lexer->Column() == 4);
  assert(lexer->Lexeme() == "");
  //  }
  assert(lexer->NextToken() == '}');
  assert(lexer->Token() == '}');
  assert(lexer->Line() == 11);
  assert(lexer->Column() == 2);
  assert(lexer->Lexeme() == "");
  // }
  assert(lexer->NextToken() == '}');
  assert(lexer->Token() == '}');
  assert(lexer->Line() == 12);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");
  assert(lexer->NextToken() == TokenEOF);
  assert(lexer->Token() == TokenEOF);
  assert(lexer->Line() == 13);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");

  // Verify that consuming past EOF works.
  assert(lexer->NextToken() == TokenEOF);
  assert(lexer->Token() == TokenEOF);
  assert(lexer->Line() == 13);
  assert(lexer->Column() == 0);
  assert(lexer->Lexeme() == "");

  delete lexer;

  return 0;
}
